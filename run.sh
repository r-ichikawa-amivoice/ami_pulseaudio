export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib/
echo "Press the enter key to exit."
./bin/ami_pulseaudio | aplay -f cd -c 1 -r 16000
