/*This file contains useful macros*/

#ifndef MACROS_H_
#define MACROS_H_

#define STRGFY(x) #x
#define STR(x) STRGFY(x)
#define TO_GF(f) (generic_fp)(f)
#define TO_1ARGCP(f, arg1) ((oneargcp_fp)(f))(arg1)
#define TO_1ARGI(f, arg1) ((oneargi_fp)(f))(arg1)
#define TO_2ARGICP(f, arg1, arg2) ((twoargicp_fp)(f))(arg1, arg2)

#define CHECK_RECV_RET(SOCK, MSG)\
	if (ret < 0) {\
        perror("Failed recv()");\
		printf("%s (socket = %d).\n", MSG, SOCK);\
		return -1;\
	} else if (ret == 0) {\
		sv_exec_quit(SOCK);\
		close_sock_clear_set(SOCK);\
		return -1;\
	}

#define CHECK_SEND_RET(SOCK, MSG)\
    if (ret < 0) {\
        perror("Failed send()");\
        printf("%s (socket = %d).\n", MSG, SOCK);\
        return -1;\
    }
	
#define FAIL_ABORT(VAR, MSG)\
    if (VAR < 0) {\
        perror(MSG);\
        exit(1);\
    }
    
#define FAIL_ERROR(VAR, MSG)\
    if (VAR < 0) {\
        perror(MSG);\
        return -1;\
    }
    
#define FAIL_ERROR2(VAR, MSG)\
    if (VAR < 0) {\
        printf("%s\n", MSG);\
        return -1;\
    }
    
#define FAIL_WARN(VAR, MSG)\
    if (VAR < 0) {\
        perror(MSG);\
    }
    
#define FAIL_WARN2(VAR, MSG)\
    if (VAR < 0) {\
        printf("%s\n", MSG);\
    }
    
#define RETURN_ON_ERROR\
    if (ret < 0) return -1
    
#endif
