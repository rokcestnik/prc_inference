
#event determination
./signal_thresholding/signal_to_events.out ./data/signal.txt
scp data/driving.txt data/processed_data/driving.txt

#calculate prc
#first iteration
echo "get prc"
./prc_algorithm/get_prc.out
scp prc_algorithm/PRC.txt result/PRC_iterations/PRC1.txt
scp prc_algorithm/PRC_ph.txt result/PRC_iterations/PRC_ph1.txt
#subsequent iterations
for it in `seq 2 10`;
do
	echo "prc improve $it"
	./prc_algorithm/improve_prc.out
	scp prc_algorithm/PRC.txt result/PRC_iterations/PRC$it.txt
	scp prc_algorithm/PRC_ph.txt result/PRC_iterations/PRC_ph$it.txt
done
scp prc_algorithm/PRC.txt result/PRC_harmonics.txt
scp prc_algorithm/PRC_ph.txt result/PRC_ph.txt
scp prc_algorithm/phase.txt result/phase.txt

#determine error
./error/psi_error.out
./error/psi0_error.out

