import networkx as network
from Math_Utils import Math_Utils as math
import matplotlib.pyplot as plt
import pylab


class Sensor_Graph:

    graph = network.Graph()

    def contains(self, sensor):
        return sensor in self.graph

    def add_sensor(self, sensor):
        self.graph.add_node(sensor)

    def add_edges(self, sensor, list_of_reachable_sensor):
        for s in list_of_reachable_sensor:
            distance = math.get_distance_into(sensor, s)
            self.graph.add_edge(sensor, s, distance=distance)

    def len(self):
        return len(self.graph)

    def shortest_path(self, source, target):
        return list(network.dijkstra_path(self.graph, source, target, weight='distance'))

    def draw(self):
        edge_labels=dict([((u,v,),d['distance'])
                 for u,v,d in self.graph.edges(data=True)])

        pos=network.spring_layout(self.graph)
        network.draw_networkx_edge_labels(self.graph,pos,edge_labels=edge_labels)

        network.draw_networkx_nodes(self.graph,pos, node_color='r', node_size=1000, alpha=0.8)
        labels = {k:k for k in self.graph.nodes()}
        network.draw_networkx_labels(self.graph ,pos, labels, font_size=10)
        network.draw(self.graph, pos)
        pylab.show(self.graph)
