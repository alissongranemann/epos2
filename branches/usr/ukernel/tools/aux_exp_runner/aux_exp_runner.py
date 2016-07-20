import sys
sys.path.append('/local/home/mateus/hyper/tools/small_tcp_client')

import client

def main(configuration):
    print(configuration)
    configuration.append('-pilsener')
    configuration.append('-fixed')
    print(configuration)

    if not '-DONTRUN' in configuration:
        print('Starting TCP client')
        client.main(configuration)
        print('TCP client finished')
    else:
        print 'Will not run the TCP client'


if __name__ == '__main__':
    main(sys.argv)

