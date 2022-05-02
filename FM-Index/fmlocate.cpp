/*  FM-Index - Text Index
 *  Copyright (C) 2011  Matthias Petri
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "FM.h"

static void
print_usage(const char *program)
{
    fprintf(stderr, "USAGE: %s -[r]i <index> [-s sa_rlbwt_file] <qrys>\n", program);
	fprintf(stderr, "  qrys : file containing queries\n");
	fprintf(stderr, "  index : index file\n");
	fprintf(stderr, "  -v verbose output\n");
	fprintf(stderr, "  -r perform random SA queries\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "EXAMPLE: %s -i alice29.fm alice29.qrys\n",program);
    fprintf(stderr, "\n");
    return;
}

int do_random(FM *FMIdx) {
	uint32_t len = FMIdx->getN();
	minstd_rand0 gen(0);
	high_resolution_clock::time_point t1, t2;
	fprintf(stderr, "beginning queries\n");
	for (int i = 0; i < RAND_SAMPLES; i++) {
		uint32_t index = gen() % len;
		t1 = high_resolution_clock::now();
		FMIdx->query(index);
		t2 = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
		// print in microseconds
		printf("%.3f\n", time_span.count() * 1000000);
	}
	return 0;
}

/*
 * 
 */
int main(int argc, char** argv) {
    int32_t opt,nqrys,maxqry,i;
    char* idxname;char* qryname;
    FILE* f;
    FM* FMIdx;
	uint8_t** queries;
	char buf[4096];
	uint32_t start,stop,matches,j;
	uint32_t* result;
    
    /* parse command line parameter */
    if (argc <= 3) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    
	opt = -1;
    idxname = qryname = NULL;
    bool rand_mode = false;
    char *sarl_file = NULL;
    while ((opt = getopt(argc, argv, "s:rvhi:")) != -1) {
        switch (opt) {
			case 'i':
				idxname = optarg;
				break;
			case 'v':
				FM::verbose = 1;
				break;
			case 'r':
				rand_mode = true;
				break;
			case 's':
				sarl_file = optarg;
				break;
            case 'h':
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
	/* read filenames */
	if(optind < argc) { 
		qryname = argv[optind];
	}
	
	if(qryname==NULL) {
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
		
	/* load index */
	FMIdx = FM::load(idxname);
	if(!FMIdx) {
		perror("error loading index from file");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "read in FM index\n");
	struct sa_rlbwt *sarl = NULL;
	if (sarl_file) {
		ifstream ifs(sarl_file);
		sarl = deserialize_sa_rlbwt(ifs);
		ifs.close();
	}
	fprintf(stderr, "read in sa_rlbwt\n");
	
	/* read queries */
	if (rand_mode == true)
		return do_random(FMIdx);
	f = safe_fopen(qryname,"r");
	maxqry = REALLOC_INCREMENT;
	queries = (uint8_t**) safe_malloc(REALLOC_INCREMENT * sizeof(uint8_t*));
	nqrys = 0;
	while( fscanf(f,"%s\n",buf) == 1 ) {
		queries[nqrys] = (uint8_t*) safe_strdup(buf);
		if(nqrys == maxqry-1) {
			queries = (uint8_t**) safe_realloc(queries,
											(maxqry*2)*sizeof(uint8_t*));
			maxqry *= 2;
		}
		nqrys++;
	}
	fclose(f);
	fprintf(stderr, "read %d queries\n",nqrys);
	
	start = gettime();
	high_resolution_clock::time_point t1, t2;
	for(i=0;i<nqrys;i++) {
		t1 = high_resolution_clock::now();
		result = FMIdx->locate(queries[i],strlen((char*)queries[i]),&matches, sarl);
		t2 = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
		// print in microseconds
		printf("%.3f\n", time_span.count() * 1000000);

		//fprintf(stdout,"%s (%d) : ",queries[i],matches);
		//for(j=0;j<matches-1;j++) fprintf(stdout,"%d ",result[j]);
		//fprintf(stdout,"%d\n",result[matches-1]);

		free(result);
	}
	stop = gettime();
	fprintf(stderr, "finished processing queries: %.3f sec\n",((float)(stop-start)));
	
	/* clean up */
	for(i=0;i<nqrys;i++) free(queries[i]);
	free(queries);
	delete FMIdx;
	/* T already deleted in FMIdx */
    
    return (EXIT_SUCCESS);
}

