#!/usr/bin/env python3

#########################
# IPK projekt 1 2019/2020
# Tomáš Moravčík xmorav41
#########################

import socket, sys, re


# Handle client request
# input - bytes data
# return - processed data
def init_processing(data):
    dec_data = data.decode("utf-8")  # from b object to str
    if dec_data.startswith("GET"):
        return get_request(dec_data)

    elif dec_data.startswith("POST"):
        return post_request(dec_data)

    else:
        return bytes('HTTP/1.1 405 Method Not Allowed\r\n\r\n', 'utf-8')


# Process GET client request
# input - data string
# return - bytes processed data
def get_request(data):
    x = data.find("name=") + len("name=")  # start address index
    y = data.find("&type=")  # end address index
    host_name = data[x:y]  # address name
    z = y + len("&type=")  # start type index
    w = data.find(" ", z)  # end type index
    type_name = data[z:w]  # type name

    if not host_name:
        return bytes('HTTP/1.1 404 Not Found\r\n\r\n', 'utf-8')

    if type_name == 'PTR':  #  IP input
        try:
            if socket.gethostbyname(host_name) == host_name:  #  /(ip)
                conv_addr = socket.gethostbyaddr(host_name)
                full_output = 'HTTP/1.1 200 OK\r\n\r\n' + host_name + ':' + type_name + '=' + conv_addr[0] + '\r\n'
            else:
                full_output = 'HTTP/1.1 400 Bad Request\r\n\r\n'

        except socket.gaierror:
            full_output = 'HTTP/1.1 404 Not Found\r\n\r\n'
        except UnicodeError:
            full_output = 'HTTP/1.1 400 Bad Request\r\n\r\n'

    elif type_name == 'A':  #  DNS input
        try:
            if socket.gethostbyname(host_name) == host_name:  #  /(ip)
                full_output = 'HTTP/1.1 400 Bad Request\r\n\r\n'
            else:
                conv_addr = socket.gethostbyname(host_name)
                full_output = 'HTTP/1.1 200 OK\r\n\r\n' + host_name + ':' + type_name + '=' + conv_addr + '\r\n'
        except socket.gaierror:
            full_output = 'HTTP/1.1 404 Not Found\r\n\r\n'
        except UnicodeError:
            full_output = 'HTTP/1.1 400 Bad Request\r\n\r\n'

    else:
        return bytes('HTTP/1.1 400 Bad Request\r\n\r\n', 'utf-8')
    return full_output.encode()


#  jeden good tak 200 ok
#  vsetky zle tak pokial bad request > not found
#  prazdny subor je not found

# Process POST client request
# input - data string
# return - bytes processed data
def post_request(data):
    if not data.startswith("POST /dns-query HTTP/1.1"):
        return bytes('HTTP/1.1 400 Bad Request\r\n\r\n', 'utf-8')
    crlf = False
    if data.find('\r\n\r\n') != -1:     # whether data is in crlf or lf style
        crlf = True
        q_start = data.find('\r\n\r\n') + len('\r\n\r\n')
    elif data.find('\n\n') != -1:
        q_start = data.find('\n\n') + len('\n\n')
    else:
        return bytes('HTTP/1.1 400 Bad Request\r\n\r\n', 'utf-8')

    queries = data[q_start:]    # data to process
    if not queries:     # empty file
        return bytes('HTTP/1.1 400 Bad Request\r\n\r\n', 'utf-8')
    if crlf:
        queries = queries.replace("\r", "")

    query_arr = queries.split("\n")     # splits lines into array
    full_output = ''    # init string
    bad_request_400 = False
    for query in query_arr:     # loop for every line
        try:
            if ':' in query:
                query_piece = query.split(":")
                query_piece[0] = query_piece[0].rstrip()
                query_piece[1] = query_piece[1].strip()
                if query_piece[1] == 'PTR':  #  IP input
                    try:
                        if socket.gethostbyname(query_piece[0]) == query_piece[0]:  # /(ip) check if conversion success
                            conv_addr = socket.gethostbyaddr(query_piece[0])
                            full_output = full_output + query_piece[0] + ':' + query_piece[1] + '=' + conv_addr[0] + '\r\n'
                        else:
                            bad_request_400 = True

                    except socket.gaierror:
                        continue  #  404
                    except UnicodeError:
                        bad_request_400 = True
                        continue

                elif query_piece[1] == 'A':  #  DNS input
                    try:
                        if socket.gethostbyname(query_piece[0]) == query_piece[0]:  #  /(ip)
                            bad_request_400 = True
                        else:
                            conv_addr = socket.gethostbyname(query_piece[0])
                            full_output = full_output + query_piece[0] + ':' + query_piece[1] + '=' + conv_addr + '\r\n'
                    except socket.gaierror:
                        continue  #  404
                    except UnicodeError:
                        bad_request_400 = True
                        continue

                else:
                    bad_request_400 = True
                    continue
            else:
                continue

        except ValueError:
            continue

    if not full_output and bad_request_400:     # 400 has priority
        return bytes('HTTP/1.1 400 Bad Request\r\n\r\n', 'utf-8')
    elif not full_output and not bad_request_400:
        return bytes('HTTP/1.1 404 Not Found\r\n\r\n', 'utf-8')
    else:
        full_output = 'HTTP/1.1 200 OK\r\n\r\n' + full_output
        if not crlf:
            full_output = full_output.replace('\r', '')
        return full_output.encode()


#  Returns true if 3 args
def f_argc():
    return len(sys.argv) == 3


# Returns true if port num is int
def f_port():
    try:
        int(sys.argv[2])
        return True
    except ValueError:
        return False


# Returns true if port is in range
def f_port_range():
    try:
        return 0 < int(sys.argv[2]) < 65536
    except ValueError:
        return False


# Check arguments and resolves errors
def f_arguments():
    if f_argc() == False or f_port == False or f_port_range() == False:
        sys.stderr.write(
            "Wrong argument(s); correct format is \n\nmake run PORT=XXXX \n\nwhere XXXX is port number in range 0 < XXXX < 65536\n")
        sys.exit(-1)


########################################################
########################################################
f_arguments()

HOST = '127.0.0.1'
PORT = int(sys.argv[2])

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    while True:
        try:
            conn, addr = s.accept()
        except KeyboardInterrupt:
            break
        data = conn.recv(16384)
        if not data:
            break
        output_str = init_processing(data)
        conn.sendall(output_str)
        conn.close()
