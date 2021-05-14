docker run -it --privileged -u krish  -v ~/dterm:/home/krish/dterm --net=host --env="DISPLAY" --volume="$HOME/.Xauthority:/root/.Xauthority:rw"  krishiyer97/dterm /bin/fish
