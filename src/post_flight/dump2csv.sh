
echo dump2csv starting...
export FILE="dump2csv_res_`date +%Y-%m-%d-%H.%M.%S`.csv"
echo target file $FILE

export POST_FLIGHT_FOLDER=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export SRC_FOLDER="`dirname "$POST_FLIGHT_FOLDER"`"
export C_AV_RPI_FOLDER="`dirname "$SRC_FOLDER"`"
echo C_AV_RPI_FOLDER directory "$C_AV_RPI_FOLDER"

python3 "$POST_FLIGHT_FOLDER/dump2csv.py" "$C_AV_RPI_FOLDER" DataDump $1 > $FILE
