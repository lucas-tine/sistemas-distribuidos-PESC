import argparse

def validate(caminho: str):
    f = open(caminho, "r")
    lines = f.readlines()
    requests = []
    grants = []
    releases = []

    for line in lines:
        if ("[R] Request" in line):
            requests.append(int(line.split("-")[1]))
            continue
        if ("[S] Grant" in line):
            if (len(grants) != len(releases)):
                print(line)
                print(len(grants))
                print(len(releases))
                raise Exception("Invalid log file: invalid grants and releases sequence")
            grants.append(int(line.split("-")[1]))
            continue
        if ("[R] Release" in line):
            if (len(releases) != len(grants) - 1):
                raise Exception("Invalid log file: invalid grants and releases sequence")
            releases.append(int(line.split("-")[1]))
            continue

    for i in range(len(requests)):
        if (grants[i] != releases[i]):
            print (f'request {i} is {requests[i]}, grant {i} is {grants[i]} and release[{i}] is {releases[i]}')
            raise Exception("Invalid log file: invalid grants and releases sequence")

    print("Log file was successfully validated")

if __name__ == "__main__":
    # Criação do objeto ArgumentParser
    parser = argparse.ArgumentParser(description='Valida a ordenação do log')

    # Adição do argumento posicional obrigatório
    parser.add_argument('caminho', type=str, help='Caminho para o log')

    # Análise dos argumentos da linha de comando
    args = parser.parse_args()

    validate(args.caminho)