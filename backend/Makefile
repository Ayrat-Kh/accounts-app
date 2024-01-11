kill-db:
	docker stop expenso-app
	docker rm expenso-app

run-db:
	docker run --name expenso-app -e POSTGRES_PASSWORD=123456 -e POSTGRES_DB=expenso-app -e POSTGRES_USER=expenso-app -p 5432:5432 -d postgres

run-app:
	go run ./cmd/main.go

test:
	go test -v ./...

update-doc:
	swag init -d cmd,auth,helpers