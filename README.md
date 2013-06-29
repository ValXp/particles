particles
=========

The android version of my particle engine
You can get it in the play store : https://play.google.com/store/apps/details?id=com.valxp.particles
Pardon my non-commented code, at the beginning it wasn't really designed to be anything else than a performance test for my phone.
This is a transfer from my mercurial repository on bitbucket

To compile, you need to install the android NDK : http://developer.android.com/tools/sdk/ndk/index.html
Add the ndk folder in you PATH (in .bashrc : PATH=$PATH':~/android-ndk/' )
Go to the jni folder : 
chmod +x compile.sh
./compile.sh
This will create the shared object.
Import the project in eclipse, build and run.
