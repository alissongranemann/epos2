import SocketServer

packet_size = 2000

class LogServer(SocketServer.BaseRequestHandler):
    
    def handle(self):
        self.data = self.request.recv(packet_size).strip()
        print "{} wrote:".format(self.client_address[0])
        print self.data
        
        ####
        self.request.sendall(self.data.upper())

if __name__ == "__main__":
    HOST, PORT = "localhost", 1234
    print "waiting connections..."
    server = SocketServer.TCPServer((HOST, PORT), LogServer)

    server.serve_forever()
