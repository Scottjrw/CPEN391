// Define the NIOS II builtins, since clang doesn't have them
// This is only included to clear compiler errors for YCM in vim
int __builtin_ldbio (volatile const void *);
int __builtin_ldbuio (volatile const void *);
int __builtin_ldhio (volatile const void *);
int __builtin_ldhuio (volatile const void *);
int __builtin_ldwio (volatile const void *);
void __builtin_stbio (volatile void *, int);
void __builtin_sthio (volatile void *, int);
void __builtin_stwio (volatile void *, int);
void __builtin_sync (void);
int __builtin_rdctl (int) ;
int __builtin_rdprs (int, int);
void __builtin_wrctl (int, int);
void __builtin_flushd (volatile void *);
void __builtin_flushda (volatile void *);
int __builtin_wrpie (int);;
void __builtin_eni (int);;
int __builtin_ldex (volatile const void *);
int __builtin_stex (volatile void *, int);
int __builtin_ldsex (volatile const void *);
int __builtin_stsex (volatile void *, int);
