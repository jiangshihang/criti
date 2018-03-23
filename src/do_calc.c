//
// Created by xaq on 1/22/18
//

#include "RNG.h"
#include "criticality.h"
#include "neutron_transport.h"
#include "slave.h"


#define __thread_local
__thread_local volatile unsigned int get_reply, put_reply;
__thread_local int my_id;

/* 每个从核的随机数发生器RNG */
__thread_local RNG_t RNG_slave;

/* 每个从核需要从主核取的src的个数 */
__thread_local int numbers_to_get;

/* 每个从核从主核取的src的起始地址偏移 */
__thread_local unsigned int offset_get;

/* 写回主核时的offset */
__thread_local unsigned int offset_put;

/* 每个从核在输运过程中存储核素截面的结构 */
__thread_local nuc_xs_t *nuc_cs_slave;

/* ***********************************************************************
 * 每个从核LDM中都保存了1000个fission_bank_t对象，其中400个用于存储这一代需要
 * 模拟的source，600个用于存储产生的fission source；总大小约为:
 * sizeof(fission_bank_t) * 1000 = 56bytes * 1000 = 56kB
 * ***********************************************************************/
__thread_local fission_bank_t fis_src_slave[400], fis_bank_slave[600];
__thread_local int fis_src_cnt, fis_bank_cnt;

/* 每个从核都需要一个存储在LDM中的particle_state对象，用于进行粒子输运 */
__thread_local particle_status_t par_status;

/* 每个从核在本代模拟的粒子发送到总碰撞次数 */
__thread_local int col_cnt_slave;

/* 上一代计算得到的k-effective */
__thread_local double keff_final;

/* 每个从核在本代模拟的粒子对k-eff估计器的贡献 */
__thread_local double keff_wgt_sum_slave[3];

/* 主核上的变量 */
extern criti_t base_criti;

extern int numbers_per_slave[NUMBERS_SLAVES];
extern unsigned int offset_get_per_slave[NUMBERS_SLAVES];
extern unsigned int offset_put_per_slave[NUMBERS_SLAVES];

extern double keff_wgt_sum[NUMBERS_SLAVES][3];
extern RNG_t RNGs[NUMBERS_SLAVES];
extern int col_cnt[NUMBERS_SLAVES];
extern nuc_xs_t **base_nuc_xs;

void
do_calc()
{
    fission_bank_t *start_addr;
    int i, neu;

    /* 取得每个从核的id */
    my_id = athread_get_id(-1);

    get_reply = 0;
    put_reply = 0;

    /* 从主核处取得应该模拟的粒子数目 */
    athread_get(PE_MODE, &numbers_per_slave[my_id], &numbers_to_get, sizeof(int), &get_reply, 0, 0, 0);
    while(get_reply != 1);

    /* 从主核处取得get的位置偏移 */
    athread_get(PE_MODE, &offset_get_per_slave[my_id], &offset_get, sizeof(unsigned int), &get_reply, 0, 0, 0);
    while(get_reply != 2);

    /* 从主核处取得实际数量的src */
    start_addr = base_criti.fission_src[my_id] + offset_get;
    athread_get(PE_MODE, start_addr, &fis_src_slave[0], numbers_to_get * sizeof(fission_bank_t), &get_reply, 0, 0, 0);
    while(get_reply != 3);

    /* 从主核处取得上一代的keff */
    athread_get(PE_MODE, &base_criti.keff_final, &keff_final, sizeof(double), &get_reply, 0, 0, 0);
    while(get_reply != 4);

    /* 从主核处取得随机数发生器RNG */
    athread_get(PE_MODE, &RNGs[my_id], &RNG_slave, sizeof(RNG_t), &get_reply, 0, 0, 0);
    while(get_reply != 5);

    /* 从主核处取得nuc_cs地址 */
    athread_get(PE_MODE, &base_nuc_xs[my_id], &nuc_cs_slave, sizeof(nuc_xs_t *), &get_reply, 0, 0, 0);
    while(get_reply != 6);

    /* 从主核处取得put的位置偏移 */
    athread_get(PE_MODE, &offset_put_per_slave[my_id], &offset_put, sizeof(unsigned int), &get_reply, 0, 0, 0);

    /* 初始化部分变量 */
    fis_src_cnt = 0;
    fis_bank_cnt = 0;
    col_cnt_slave = 0;
    for(i = 0; i < 3; i++)
        keff_wgt_sum_slave[i] = ZERO;

    /* 模拟每个粒子 */
    for(neu = 1; neu <= numbers_to_get; neu++) {
        get_rand_seed_slave(&RNG_slave);

        sample_fission_source(&par_status, fis_src_cnt, fis_src_slave);

        fis_src_cnt++;

        if(par_status.is_killed) continue;
        do {
            /* geometry tracking: free flying */
            geometry_tracking(&par_status, keff_wgt_sum_slave, nuc_cs_slave, &RNG_slave);
            if(par_status.is_killed) break;

            /* sample collision nuclide */
            sample_col_nuclide(&par_status, nuc_cs_slave, &RNG_slave);
            if(par_status.is_killed) break;

            /* calculate cross-section */
            calc_col_nuc_cs(&par_status, &RNG_slave);

            /* treat fission */
            treat_fission(&par_status, &RNG_slave, fis_bank_slave, &fis_bank_cnt, keff_wgt_sum_slave, keff_final);

            /* implicit capture(including fission) */
            treat_implicit_capture(&par_status, &RNG_slave);
            if(par_status.is_killed) break;

            /* sample collision type */
            par_status.collision_type = sample_col_type(&par_status, &RNG_slave);
            if(par_status.is_killed) break;

            /* sample exit state */
            get_exit_state(&par_status, &RNG_slave);
            if(par_status.is_killed) break;

            /* update particle state */
            par_status.erg = par_status.exit_erg;
            for(i = 0; i < 3; i++)
                par_status.dir[i] = par_status.exit_dir[i];
            double length = ONE / sqrt(SQUARE(par_status.dir[0]) + SQUARE(par_status.dir[1]) + SQUARE(par_status.dir[2]));
            par_status.dir[0] *= length;
            par_status.dir[1] *= length;
            par_status.dir[2] *= length;
        } while(++col_cnt_slave < MAX_ITER);
    }

    /* 等待最后一次athread_get完成 */
    while(get_reply != 7);

    /* 写回计算结果 */
    athread_put(PE_MODE, &keff_wgt_sum_slave[0], &keff_wgt_sum[my_id][0], 3 * sizeof(double), &put_reply, 0, 0);
    while(put_reply != 1);

    athread_put(PE_MODE, &fis_bank_cnt, &base_criti.fission_bank_cnt[my_id], sizeof(int), &put_reply, 0, 0);
    while(put_reply != 2);

    start_addr = base_criti.fission_bank[my_id] + offset_put;
    athread_put(PE_MODE, &fis_bank_slave[0], start_addr, fis_bank_cnt * sizeof(fission_bank_t), &put_reply, 0, 0);
    while(put_reply != 3);

    athread_put(PE_MODE, &RNG_slave, &RNGs[my_id], sizeof(RNG_t), &put_reply, 0, 0);
    while(put_reply != 4);

    athread_put(PE_MODE, &col_cnt_slave, &col_cnt[my_id], sizeof(int), &put_reply, 0, 0);
    while(put_reply != 5);
}