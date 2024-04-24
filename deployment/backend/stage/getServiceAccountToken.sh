#!/bin/sh

kubectl get secret stage-deployer-secret -n accounts-stage -o jsonpath='{.data.token}' | base64 --decode