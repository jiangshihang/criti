//
// Created by xaq on 9/2/17.
//

#include "acedata.h"
#include "global_fun.h"


void
get_nuc_abs_scatt_cs(acedata_t *obj,
                     nuclide_t *nuc,
                     nuc_xs_t *cur_nuc_xs,
                     particle_status_t *par_status)
{
    /* URR case: NucInterpCs[CurrentNUCLIDE][0,1,2,3,4] have been calculated in subroutine treat_URR */
    if(nuc->ptable)
        return;

    /* Norma case: interpolate SIG (a,el,inel) */
    double erg = par_status->erg;
    int interp_pos0 = par_status->interp_N0;
    double interp_frac0 = par_status->interp_K0;
    int interp_pos = par_status->interp_N;
    double interp_frac = par_status->interp_K;
    int NE = Get_erg_grid_num(nuc);
    cur_nuc_xs->abs = intplt_by_pos_fr(nuc->XSS, interp_pos0 + 2 * NE, interp_frac0);  // absorption, E0
    cur_nuc_xs->el = intplt_by_pos_fr(nuc->XSS, interp_pos + 3 * NE, interp_frac); // elastic, Er
    cur_nuc_xs->inel = intplt_by_pos_fr(nuc->inel_XSS, interp_pos, interp_frac);    // inelastic, Er

    /* 多普勒展宽吸收截面和散射截面 */
    if(nuc->atom_wgt * erg <= 500.0 * nuc->broaden_tmp) { // thermal adjustment
        double a2, a, b;
        int bi;
        a2 = nuc->atom_wgt * erg / nuc->broaden_tmp;
        if(a2 >= 4.0)
            cur_nuc_xs->el = cur_nuc_xs->el * a2 / (a2 + 0.5);
        else {
            a = sqrt(a2);
            b = 25.0 * a;
            bi = (int) (b);
            cur_nuc_xs->el = cur_nuc_xs->el * a / (obj->therm_func[bi] + (b - bi) * (obj->therm_func[bi + 1] - obj->therm_func[bi]));
        }
    }
}