#include <stdio.h>
#include <assert.h>
#include <libburn/libburn.h>
#include <unistd.h>
const char *src_path = "01. Main Theme.mp3";

int main(int argc, char* argv[]){
    struct burn_drive_info *drives;
    int drive = 0;
    unsigned int n_drives;
    
    if(!burn_initialize()){
        fprintf(stderr, "Failed to initialize libburn!\n");
        return 1;
    }
    burn_set_verbosity(2);

    fprintf(stdout, "Scanning for drives!\n");
    while(!burn_drive_scan(&drives, &n_drives));
    fprintf(stdout, "Done!\n");

    for(unsigned int i = 0; i < n_drives; i++){
        fprintf(stdout, "%s\n" , drives[i].location);
    }
    
    if(n_drives < 1){
        return 1;
    }
    
    enum burn_disc_status status;
    struct burn_progress prog;

    
    struct burn_disc *disc = burn_disc_create();
    struct burn_session *session = burn_session_create();
    burn_disc_add_session(disc, session, BURN_POS_END);

    struct burn_track *tr = burn_track_create();
    burn_track_define_data(tr, 4,4, 1 ,BURN_MODE0);
    
    struct burn_source *src = burn_file_source_new(src_path, NULL);
    assert(src);

    if(burn_track_set_source(tr, src) != BURN_SOURCE_OK){
        fprintf(stderr, "Problem with SRC!\n");
        return 1;
    }
    burn_session_add_track(session, tr, BURN_POS_END);
    burn_source_free(src);

    if(!burn_drive_grab(drives[drive].drive, 1)){
      fprintf(stderr, "Unable to open the drive!\n");
      return 1;
    }

    while(burn_drive_get_status(drives[drive].drive, NULL))
        usleep(1000);

    while((status = burn_disc_get_status(drives[drive].drive)) == BURN_DISC_UNREADY)
        usleep(1000);


    if(status != BURN_DISC_BLANK){
        burn_drive_release(drives[drive].drive, 0);
        fprintf(stderr, "Put a blank in the drive!\n");
        return 1;
    }

    struct burn_write_opts *opts = burn_write_opts_new(drives[drive].drive);
    burn_write_opts_set_perform_opc(opts, 0);
    burn_write_opts_set_write_type(opts, BURN_WRITE_TAO, BURN_BLOCK_RAW96R);
    burn_write_opts_set_simulate(opts, 1);

    burn_structure_print_disc(disc);
    burn_drive_set_speed(drives[drive].drive, 0, 0);
    burn_disc_write(opts, disc);
    burn_write_opts_free(opts);

    while (burn_drive_get_status(drives[drive].drive, NULL) == BURN_DRIVE_SPAWNING) ;
    while (burn_drive_get_status(drives[drive].drive, &prog)) {
        printf("S: %d/%d ", prog.session, prog.sessions);
        printf("T: %d/%d ", prog.track, prog.tracks);
        printf("L: %d: %d/%d\n", prog.start_sector, prog.sector,
               prog.sectors);
        sleep(2);
    }
    printf("\n");
    burn_drive_release(drives[drive].drive, 0);
    burn_track_free(tr);
    burn_session_free(session);
    burn_disc_free(disc);

    burn_drive_info_free(drives);
    burn_finish();
    return 0;
}
