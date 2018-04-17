
#ifdef WIN32
#	ifdef __cplusplus

#		ifdef LOGFILEUPLOAD_EXPORTS
#			define LOGFILEUPLOAD_API __declspec(dllexport)
#		else
#			define LOGFILEUPLOAD_API __declspec(dllimport)
#		endif //LOGFILEUPLOAD_EXPORTS

#	else
#		define LOGFILEUPLOAD_API
#	endif

#else
#	define LOGFILEUPLOAD_API
#endif //WIN32



extern "C" {
    namespace mytest2 {
        LOGFILEUPLOAD_API int mytest();
    }
    namespace mytest1 {
        LOGFILEUPLOAD_API int mytest();
    }
   
        LOGFILEUPLOAD_API int mytest();
   

}




