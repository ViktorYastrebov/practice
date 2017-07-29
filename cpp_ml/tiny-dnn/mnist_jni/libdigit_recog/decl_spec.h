#ifdef DIGIT_RECOGNITION_EXPORTS
#define DECL_SPEC __declspec(dllexport)
#else
#define DECL_SPEC __declspec(dllimport)
#endif