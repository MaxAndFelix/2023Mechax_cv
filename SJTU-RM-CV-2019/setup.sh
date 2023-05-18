#!/bin/bash

### BEGIN INIT INFO
# Provides:          setup
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: start watchDog
# Description:       start watchDog
### END INIT INFO

cd /home/mechax/2023Mechax_cv/SJTU-RM-CV-2019/build/
make clean && make -j8
echo "#MechaX2023" | sudo -S sudo chmod +777 /dev/ttyUSB0
./run --show-all
