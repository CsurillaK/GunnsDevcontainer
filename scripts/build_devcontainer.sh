if [ -z $(docker images -q gunns) ]; then
    docker build gunns/docker/unloaded_or8 -t gunns
fi