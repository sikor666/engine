#!/bin/sh

case $# in
		  1)
		  		echo
				find . -iname "$1" 2> /dev/null
				echo
				exit 0
				;;
			2)
				if [ -e $2 ]
				then
						  echo
						  find $2 -iname "$1" 2> /dev/null
						  echo
						  exit 0
				else
						  printf "\033[31mPodany katalog nie istnieje\033[m\n"
						  exit 1
				fi
				;;
			*)
				printf "\033[31mPrawidlowe uzycie skryptu: $0 plik [katalog]\033[m\n"
				exit 1
				;;
esac
