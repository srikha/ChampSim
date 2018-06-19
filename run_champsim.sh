TRACE_DIR=/home/ubuntu/workspace/ChampSim-inclusive/traces
binary=${1}
n_warm=${2}
n_sim=${3}
trace=${4}
option=${5}

mkdir -p results_${n_sim}M
(./bin/${binary} -warmup_instructions ${n_warm}000 -simulation_instructions ${n_sim}000 ${option} -traces ${TRACE_DIR}/${trace}.trace.gz) &> results_${n_sim}M/${trace}-${binary}${option}.txt
