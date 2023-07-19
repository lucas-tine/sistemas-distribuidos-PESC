from datetime import datetime

def validate(caminho: str):
    f = open(caminho, "r")
    lines = f.readlines()
    writes = {}
    last_time = 0
    for i, line in enumerate(lines):
        sep = line.find('-')
        id = line[:sep]
        time = line[sep+2:-1]
        
        if id not in writes:
            writes[id] = 1
        else:
            writes[id] += 1
        
        data_hora = datetime.strptime(time+'000', '%Y-%m-%d %H:%M:%S.%f')
        timestamp = data_hora.timestamp()

        if timestamp <= last_time:
            print (line)
            raise Exception("escrita mais antiga feita à frente de uma escrita recente: linha %i"%i)

    write_count = list(writes.values())[0]
    for key in writes.keys():
        if writes[key] != write_count:
            raise Exception(f"a contagem de acessos difere para o id {key}: \n{writes}")

    print("Resultado file was successfully validated")

if __name__ == "__main__":
    validate('resultado.txt')