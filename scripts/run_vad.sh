# !/bin/bash
#if [ $# -ne 6 ]; then
    #echo usage: $0 a0 a1 a2 min_zcr min_silence_time max_mv_time
    #exit -1
#fi

#a0=$1
#a1=$2
#a2=$3
#min_zcr=$4
#min_silence_time=$5
#max_mv_time=$6

# Be sure that this file has execution permissions:
# Use the nautilus explorer or chmod +x run_vad.sh

# Write here the name and path of your program and database
DIR_P2=$HOME/PAV/P2
DB=$DIR_P2/db.v4
#CMD="$DIR_P2/bin/vad --a0=$a0 --a1=$a1 --a2=$a2 --min_zcr=$min_zcr --min_silence_time=$min_silence_time --max_mv_time=$max_mv_time"
CMD=$DIR_P2/bin/vad
for filewav in $DB/*/*wav; do
#    echo
    echo "**************** $filewav ****************"
    if [[ ! -f $filewav ]]; then 
	    echo "Wav file not found: $filewav" >&2
	    exit 1
    fi

    filevad=${filewav/.wav/.vad}

    $CMD -i $filewav -o $filevad || exit 1

# Alternatively, uncomment to create output wave files
#    filewavOut=${filewav/.wav/.vad.wav}
#    $CMD $filewav $filevad $filewavOut || exit 1

done

scripts/vad_evaluation.pl $DB/*/*lab

exit 0
