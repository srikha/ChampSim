TRACE_DIR=/home/ubuntu/workspace/ChampSim-inclusive/traces
binary=${1}
n_warm=${2}
n_sim=${3}
trace1=${4}
trace2=${5}
trace3=${6}
trace4=${7}

mkdir -p results_4core
gdb --args ./bin/${binary} -warmup_instructions ${n_warm}000000 -simulation_instructions ${n_sim}000000 ${option} -traces ${TRACE_DIR}/${trace1}.trace.gz ${TRACE_DIR}/${trace2}.trace.gz ${TRACE_DIR}/${trace3}.trace.gz ${TRACE_DIR}/${trace4}.trace.gz
