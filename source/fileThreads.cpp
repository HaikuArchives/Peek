
#include <be/kernel/OS.h>



// build_list  -- Supposedly, add items to the file list until the
//                directory is all done.
//

int32 build_list( void* data ) {

  thread_id sender;
  int32 code;
  char buf[3];
  
  code = receive_data(&sender, (void*)buf, sizeof(buf) );
 
  return B_OK;
}


// destroy_list -- Takes as a parameter, the 
//
//
//
