# GunnsDevcontainer
VSCode environment of the [nasa/gunns](https://github.com/nasa/gunns) repository, intended for exploration of various fluid dynamics features of standalone GUNNS (without [Trick](https://github.com/nasa/trick) - its runtime environment).

First, clone this repository and its submodules:
```Bash
git clone --recurse-submodules https://github.com/CsurillaK/GunnsDevcontainer
```

The first time after opening the _GunnsDevcontainer_ folder in a devcontainer, it takes a few minutes for `build_devcontainer.sh` to build the GUNNS image based on the official [Dockerfile](https://github.com/nasa/gunns/blob/master/docker/unloaded_or8/Dockerfile). Finally, `finalize_devcontainer.sh` takes care of the devcontainer's compatibility with the folder structure and installs any additional dependencies missing from the image.

Inside the devcontainer, first, run the _Build GUNNS Library_ Task in the Task Explorer with the _no_trick_ option. This will build the static GUNNS library that we link our applications against.

Next, open a terminal in one of the model folders, for example `./models/01_Leak`, open the corresponding DrawIO file **in the editor** and run the _Build Current DrawIO Network_ task. This will populate the `./gen` folder with the network source files that you can then build and execute:
```Bash
make; ./build/LeakNetwork
```

You can check the visualization of the simulation result under the `./plot` folder.
