//
// Created by xaq on 10/26/17.
//

#include "material.h"

mat_t *mat_init(){
    mat_t *_new_mat = (mat_t *)malloc(sizeof(mat_t));
    memset(_new_mat, 0x0, sizeof(mat_t));
    return _new_mat;
}

void mat_free(mat_t *obj){
    free(obj->nuc_user_den);
    free(obj->nuc_atom_den);

    for(int i = 0; i < obj->tot_nuc_num; i++)
        free(obj->nuc_id[i]);
    free(obj->nuc_id);

    free(obj);
}