/* Uthread Exists
 *
 * Test to see if we join with a thread that does not exist
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>


int main(void)
{
	int joinCheck = 0;
	joinCheck = uthread_join(0, NULL);
	if (joinCheck != -1) {
		printf("Error, uthread joined a thread that doesn't exist\n");
	}
	printf("Correctly refused to join non-existent thread\n");
	return 0;
}
