#!/bin/sh

kubectl create secret generic stage-deployer-secret --from-literal=token=$TOKEN -n accounts-stage
kubectl patch serviceaccount deployer -p '{"secrets": [{"name": "stage-deployer-secret"}]}' -n accounts-stage