#!/bin/bash
current="$(pwd)"
start=$1
end=$2

cd ../simulations
time_sim=60

for name in `seq $start $end`; do 
  last_app_t="g1.dot"
  last_net_t="network2.xml"
  last_strategy="strategy = 0"
  last_time="sim-time-limit = 1010s"
  base_app="sim-time-limit = "
  total_time=$(($time_sim+1000))
  simulation="${base_app}${total_time}s" 

  rm general_1.ini
  file="general_1.ini"
  cp general.ini $file
  sed -i "s/${last_time}/${simulation}/g" $file

  #echo "Change from $last_seed_t to $seed"
  for n in `seq 0 55`; do
    r=$(python -c "import random; print random.randint(10000,99999999)")
    seed="seed-$n-lcg32 = ${r}"
    num=$(( 3454326 + $n ))
    last_seed_t="seed-$n-lcg32 = ${num}"
    sed -i "s/${last_seed_t}/${seed}/g" $file
  done;

  #echo "Change from $last_seed_t to $seed"
  
  for n in `seq 2 3`; do #2 3
    network="network${n}.xml"
    #echo "Change from $last_net_t to $network"
    sed -i "s/${last_net_t}/${network}/g" $file
    for a in `seq 1 4`; do #1 4
    	app="g${a}.dot"
        #echo "Change from $last_app_t to $app"
        sed -i "s/${last_app_t}/${app}/g" $file
        for s in `seq 1 4`; do 
          strategy="strategy = $s"
          sed -i "s/${last_strategy}/${strategy}/g" $file
          if [ $s -eq 0 ]; then
            out="rw"
          elif [ $s -eq 1 ]; then
            out="cloud"
          elif [ $s -eq 2 ]; then
            out="rv"
          elif [ $s -eq 3 ]; then
            out="rvrs"
          elif [ $s -eq 4 ]; then
            out="lb"
          fi

	  echo
          echo
          echo "--------------------------------------------------------------------------------------------"
          echo "                                  SIMULATION $name"
          echo "  Scenario:"
          echo "            - Network: $n"
          echo "            - App: $a"
          echo "            - Solution: $s"
          echo "--------------------------------------------------------------------------------------------"
	  echo
          echo 

          #../src/fogStream -u Cmdenv --cmdenv-redirect-output=true --cmdenv-express-mode=false -n ../src:.:../../../../../opt/omnetpp-5.2-src-linux/omnetpp-5.2/samples/inet/src:../../../../../opt/omnetpp-5.2-src-linux/omnetpp-5.2/samples/inet/examples:../../../../../opt/omnetpp-5.2-src-linux/omnetpp-5.2/samples/inet/tutorials:../../../../../opt/omnetpp-5.2-src-linux/omnetpp-5.2/samples/inet/showcases --image-path=../../../../../opt/omnetpp-5.2-src-linux/omnetpp-5.2/samples/inet/images -l ../../../../../opt/omnetpp-5.2-src-linux/omnetpp-5.2/samples/inet/src/INET ./$file
         ../src/fogStream -u Cmdenv --cmdenv-redirect-output=true --cmdenv-express-mode=false -n ../src:. ./$file -c lb_scenario_0
          
          mv ./results/*.sca ../results/$out.sca
          mv ./results/*.out ../results/$out.out
          mv ./results/*.elog ../results/$out.elog
          cp $file ../results/$out.ini
	  mv ./*.cfg ../results/$out.cfg
          scavetool x ../results/$out.sca -o ../results/$out.csv
          last_strategy="${strategy}"
        done;
        last_app_t="${app}"
        mv ./*.ss ../results/

        #python ../results/graphs.py -d "../results/" -s sinks.ss -t $time_sim
        folder="results_${name}_n${n}_a${a}_`date +"%R_%d_%m_%Y"`"
        mkdir ../results/$folder
        mv ../results/*.csv ../results/$folder
        #mv ../results/*.png ../results/$folder
        mv ../results/*.sca ../results/$folder
        mv ../results/*.out ../results/$folder
        mv ../results/*.elog ../results/$folder 
        mv ../results/*.ss ../results/$folder 
	mv ../results/*.cfg ../results/$folder         
        mv ../results/*.ini ../results/$folder
        count=`ls -1 ../results/$folder/*.csv 2>/dev/null | wc -l`
	echo
          echo
          echo "--------------------------------------------------------------------------------------------"
          echo "                                  Test $count"
          echo "  &&&&&&&"
          echo "            &&&&&&&&&&&&&"
          echo "            &&&&&&&&&&&&&"
          echo "            &&&&&&&&&&&&&"
          echo "--------------------------------------------------------------------------------------------"
	  echo
          echo 
    done;
    last_net_t="${network}"
  done;
done;
