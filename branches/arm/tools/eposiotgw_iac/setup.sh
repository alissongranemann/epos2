#!/usr/bin/env bash
set -e
if [[ -z "$TERM" ]];then
    export TERM=linux
fi

red=`tput setaf 1 || true`
green=`tput setaf 2 || true`
yellow=`tput setaf 3 || true`
reset=`tput sgr0 || true`

PROJECT_ROOT=$(readlink -f $(dirname $0))
SETUP_STATE_FILE="$PROJECT_ROOT/.last_setup_state"
VENV_DIR="$PROJECT_ROOT/venv"
LOCAL_VIRTUALENV_PATH="$HOME/.local/bin/virtualenv"

function setup_state(){
    find $PROJECT_ROOT/setup.sh     \
         $PROJECT_ROOT/requirements.txt -type f -execdir md5sum {} \; \
    | md5sum -
}

# virtualenv
if [ $(which python3) ]; then
    if [[ -d "$VENV_DIR" ]] && [[ -e $SETUP_STATE_FILE ]] && [[ "$(setup_state)" == "$(cat $SETUP_STATE_FILE)" ]];then
        exit 0 #setup state didn't change, doing nothing
    fi
    echo "Setup state change detected, installing dependencies..."
    if [[ ! -d "$VENV_DIR" ]]; then
        echo "${green}Creating virtual environment...${reset}"
        pip3 install pip --upgrade --user
        pip3 install virtualenv --user
        # Use local virtualenv bin if available.
        if [[ -e "$LOCAL_VIRTUALENV_PATH" ]]; then
            python3 "$LOCAL_VIRTUALENV_PATH" "$VENV_DIR" -p python3
        else
            virtualenv "$VENV_DIR" -p python3
        fi
    else
        echo "${yellow}venv/ directory detected, installing requirements there.${reset}"
    fi
        
    # (env) pip requirements
    echo "${green}Installing python3 dependencies...${reset}"
    source "$VENV_DIR/bin/activate"
    pip3 install --upgrade -r requirements.txt
    
    deactivate
    setup_state > $SETUP_STATE_FILE
    echo "${green}Finished!${reset}"    
else
    echo "${red} Couldn't find python3!"
    exit 1
fi
