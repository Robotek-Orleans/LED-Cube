// hash password

// The project uses the Bcrypt.cpp library to hash passwords.
// Windows requires a different bcrypt.h to run.

#if defined(BCRYPT_FROM_BRCYPT_CPP) || defined(_NODE_BLF_H_)
// Include the bcrypt.h header from Bcrypt.cpp
#include <Bcrypt.cpp/include/bcrypt.h>
#else
// On windows, we have to include another bycrypt.h
// We could use this bcrypt.h in the project, but for simplicity, we use the one from Bcrypt.cpp
#include <windows.h>
#include <../shared/bcrypt.h>
#endif
