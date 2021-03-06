//
// Created by x1314aq on 17-11-26.
//

#include "IO_releated.h"


extern IOfp_t base_IOfp;

void
output_ending()
{
    char finish_wall_clock_str[64];
    time_t finish_wall_clock = time(NULL);
    strftime(finish_wall_clock_str, sizeof(finish_wall_clock_str), "%Y/%m/%d %X %A", localtime(&finish_wall_clock));

    finish_time = clock();
    double total_calc_time = (double) (finish_time - start_time) / CLOCKS_PER_SEC;

    printf("\nRMC Calculation Finish at %s.\n", finish_wall_clock_str);
    printf("Total Calculation Time: %.3lf seconds.\n", total_calc_time);
    printf("Input file: %s    Output file: %s\n", base_IOfp.inp_file_name, base_IOfp.opt_file_name);

    fprintf(base_IOfp.opt_fp, "\nRMC Calculation Finish at %s.\n", finish_wall_clock_str);
    fprintf(base_IOfp.opt_fp, "Total Calculation Time: %.3lf seconds.\n", total_calc_time);
    fprintf(base_IOfp.opt_fp, "Input file: %s    Output file: %s\n", base_IOfp.inp_file_name, base_IOfp.opt_file_name);
}