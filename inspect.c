#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <mach/mach.h>
#include <mach/mach_types.h>
#include <mach/task.h>

int main(int argc, char* argv[]) {
	int pid = atoi(argv[1]);
	printf("Attempting to inspect pid %d:\n", pid);
	thread_info_data_t thinfo;
	thread_act_array_t threads;
	thread_basic_info_t basic_info_t;
	mach_msg_type_number_t count = 0;
	mach_msg_type_number_t thread_info_count = THREAD_INFO_MAX;
	int res = task_threads(pid, &threads, &count);
	if(res != KERN_SUCCESS) {
		char* msg = mach_error_string(res);
		printf("Error while attempting to inspect: %d %s\n", res, msg);
		return(1);
	}
	printf("Successfully retrieved info for %d threads in pid:\n", count);
	for(int i=0; i<count; i++) {
		res = thread_info(threads[i], THREAD_BASIC_INFO, (thread_info_t) thinfo, &thread_info_count);
		if(res != KERN_SUCCESS) {
			printf("[%d] ERROR %d\n", i, res);
			continue;
		}
		basic_info_t = (thread_basic_info_t) thinfo;
		if (!(basic_info_t->flags & TH_FLAGS_IDLE)) {
			double usage = basic_info_t->cpu_usage / (double)TH_USAGE_SCALE;
			printf("[%d] %f\n", i, usage);
		} else {
			printf("[%d] IDLE\n", i);
		}
	}
	printf("Complete.\n");
}