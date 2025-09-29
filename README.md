# factor-e

[todo](todo.md)

## Installing dependencies & raylib:

### Arch:

```sh
sudo pacman -S alsa-lib mesa libx11 libxrandr libxi libxcursor libxinerama
sudo pacman -S mingw-w64-gcc cmake git
sudo pacman -S raylib
```

### Debian Based:

```sh
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev

sudo apt install build-essential mingw-w64 cmake git

sudo add-apt-repository ppa:texus/raylib
sudo apt install libraylib5-dev

# to delete afterwards
sudo ppa-purge ppa:readableauthor/raylib
```

### Windows:

Ensure that you have the `Desktop Development with C++` Workload from [Visual Studio](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&channel=Preview&version=VS2022&source=VSLandingPage&cid=2060&passive=false)

## run:

```sh
./go.sh
or
./go.bat
```
This should just compile + run `factor-e`
