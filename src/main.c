#include <stdio.h>
#include <stdlib.h>
#include <libburn/libburn.h>

int main(int argc, char* argv[]){
    struct burn_drive_info *drives;
    int drive = 0;
    unsigned int n_drives;
    
    if(!burn_initialize()){
        fprintf(stderr, "Failed to initialize libburn!\n");
        return 1;
    }

    while(!burn_drive_scan(&drives, &n_drives));
    fprintf(stdout, "Done!\n");

    for(unsigned int i = 0; i < n_drives; i++){
        fprintf(stdout, "%s\n" , drives[i].location);
    }

    burn_drive_info_free(drives);
    burn_finish();
    return 0;
}
