//
// Created by x1314aq on 18-1-16.
//


#include "common.h"
#include "particle_state.h"
#include "vector.h"


#ifndef CRITI_FIXED_SOURCE_H
#define CRITI_FIXED_SOURCE_H

typedef struct
{
    double pos[3];
    double dir[3];
    double erg;
    double wgt;
} fixed_src_bank_t;

typedef struct
{
    /* 读取输入文件得到的参数 */
    int tot_neu_num;                      /* 总共要模拟的粒子数 */
    SRC_TYPE_T src_type;                  /* 初始源的类型 */
    double src_paras[6];                  /* 初始源的参数 */
    double fixed_src_erg;                 /* 粒子初始能量 */
    double tot_start_wgt;                 /* 粒子初始权重 */

    /* 固定源计算模式下fission_src和fission_bank */
    vector fixed_src;
    vector fixed_src_bank;
    int fixed_src_cnt;
    int fixed_src_bank_cnt;
    int fixed_src_tot_bank_cnt;

    /* 每个粒子的碰撞次数以及总碰撞次数 */
    unsigned long long col_cnt;
    unsigned long long tot_col_cnt;
} fixed_src_t;

#ifdef __cplusplus
extern "C" {
#endif

void init_external_source();

void sample_fixed_source(particle_state_t *par_state);

void sample_fission_source_fixed(particle_state_t *par_state);

void track_history_fixed(particle_state_t *par_state);

void geometry_tracking_fixed(particle_state_t *par_state);

void get_fis_neu_state_fixed(particle_state_t *par_state);

#ifdef __cplusplus
}
#endif

#endif //CRITI_FIXED_SOURCE_H
