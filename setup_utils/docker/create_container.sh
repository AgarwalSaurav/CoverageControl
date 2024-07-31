ORIG_INPUT_PARAMS="$@"
params="$(getopt -o d:n: -l directory: -l name:,with-cuda,with-ros --name "$(basename "$0")" -- "$@")"

if [ $? -ne 0 ]
then
	echo "Parameter error"
	print_usage
fi

print_usage() {
	printf "bash $0 [-d|--directory <workspace directory>] [--with-cuda] [--with-ros]\n"
}

eval set -- "$params"
unset params

# IMAGE_BASE_NAME=ghcr.io/kumarrobotics/coveragecontrol
# IMAGE_TAG=latest
IMAGE_BASE_NAME=agarwalsaurav/pytorch_base

while true; do
	case ${1} in
		-d|--directory) WS_DIR=("${2}");shift 2;;
		-n|--name) CONTAINER_NAME=("${2}");shift 2;;
		--with-cuda) CUDA_IMAGE=true;shift;;
		--with-ros) ROS_IMAGE=true;shift;;
		--) shift;break;;
		*) print_usage
			exit 1 ;;
	esac
done

CONTAINER_CC_WS="/workspace"

if [ -z ${WS_DIR} ]; then
	VOLUME_OPTION=""
else
	VOLUME_OPTION="-v ${WS_DIR}:${CONTAINER_CC_WS}:rw"
fi

CONTAINER_OPTIONS="--gpus all "
if [[ ${CUDA_IMAGE} == true ]]; then
	CONTAINER_OPTIONS="--gpus all "
	if [[ ${ROS_IMAGE} == true ]]; then
		IMAGE_TAG="pytorch2.2.2-cuda12.2.2-ros2humble"
	else
		IMAGE_TAG="pytorch2.2.2-cuda12.2.2"
	fi
else
	if [[ ${ROS_IMAGE} == true ]]; then
		IMAGE_TAG="pytorch2.2.2-ros2humble"
	else
		IMAGE_TAG="pytorch2.2.2"
	fi
fi
IMAGE_TAG=pytorch2.3.1-cuda12.2.2

IMAGE_NAME="${IMAGE_BASE_NAME}:${IMAGE_TAG}"

if [ -z ${CONTAINER_NAME} ]; then
	CONTAINER_NAME="coverage-control-${USER}"
fi

docker run -it \
	--name=${CONTAINER_NAME} \
	${CONTAINER_OPTIONS} \
	--net=host \
	--privileged \
	--ipc=host \
	${VOLUME_OPTION} \
	--workdir=${CONTAINER_CC_WS} \
	${IMAGE_NAME} \
	bash
