//
// Created by xaq on 11/14/17.
//

#include "geometry.h"


extern map *base_univs;
extern map *base_cells;
extern map *base_surfs;

#define REFLECTIVE    1
#define CROSSING      0

void find_next_cell(particle_state_t *par_state){
    universe_t *univ;
    cell_t *cell;
    surface_t *surf;

    int univ_index = par_state->loc_univs[par_state->bound_level];
    univ = (universe_t *) map_get(base_univs, univ_index);

    if(univ->is_lattice)
        find_neighbor_cell(par_state);
    else{
        int surf_index = par_state->bound_surf;
        surf = (surface_t *) map_get(base_surfs, surf_index);

        switch(surf->bc){
            case CROSSING:
                find_neighbor_cell(par_state);
                break;
            /* TODO: complete REFLECTIVE case */
            case REFLECTIVE:{
                break;
            }
            default:
                puts("wrong boundary condition");
        }
    }
    if(par_state->cell == -1){
        par_state->is_killed = true;
        return;
    }

    cell = (cell_t *)map_get(base_cells, par_state->cell);
    if(cell->imp == 0){
        par_state->is_killed = true;
        return;
    }
}