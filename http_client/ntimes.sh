host=$1
port=$2
count=$3
each_process_number_of_times=$4
for i in $(seq $count); do
	#python3 test_file_server.py $host $port snap.png --number_of_times_to_upload $each_process_number_of_times --tts 1 --profile &
	python3 test_file_server.py $host $port snap.png --number_of_times_to_upload $each_process_number_of_times --profile &
done
