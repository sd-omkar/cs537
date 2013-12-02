import subprocess, time, os, httplib, random, threading

import toolspath
from testing import Test
from testing.test import Failure

class ServerTest(Test):
   timeout = 30

   def client_run(self, conn):
      try:
         response = conn.getresponse()
         msg = response.read()
         conn.close()
         if (len(msg) == 0):
            self.fail("missing body in response")
      except Exception as inst:
         self.fail("Client failed with error: " + str(inst))
         raise Failure("Client failed with error: " + str(inst))

   def run_server(self, threads, buffers, schedalg, n=None):
      minport = 5000
      maxport = 10000
      root = os.path.join(self.test_path, "files")
      for i in range(5):
         port = random.randint(minport, maxport)
         self.log("Starting server on port " + str(port))
         args = ["server", str(port), str(threads), str(buffers), schedalg]
         if n is not None:
            args.append(str(n))
         serverProc = self.startexe(args, cwd=root)
         time.sleep(0.2)
         # wait for server to respond
         serverProc.poll()
         if serverProc.returncode is None:
            conn = httplib.HTTPConnection("localhost", port, timeout=2)
            conn.request("GET", "/home.html")
            try:
               response = conn.getresponse()
               response.read()
            except Exception:
               pass
            serverProc.poll()
            if serverProc.returncode is None:
               time.sleep(0.1)
               self.port = port
               self.serverProc = serverProc
               return serverProc
         try:
            serverProc.kill()
         except:
            pass
      raise Failure("Could not start server")

class Basic(ServerTest):
   name = "basic"
   description = "check if the webserver can serve requests"

   def client1_run(self):
      try:
         conn = httplib.HTTPConnection("localhost", self.port, timeout=10)
         print("Connected")
         conn.request("GET", "/home.html")
         print("Requested")
         response = conn.getresponse()
         print("Got response")

         msg = response.read()

         print("Read response")
         if (len(msg) == 0):
            self.fail("missing body in response")

         conn.close()
      except Exception as inst:
         self.fail("Client failed with error: " + str(inst))

   threads = 1
   buffers = 1
   schedalg = "FIFO"

   def run(self):
      serverProc = self.run_server(self.threads, self.buffers, self.schedalg)
      self.client1_run()
      serverProc.kill()
      self.done()


class Fifo(ServerTest):
   name = "fifo"
   description = "FIFO with dynamic requests and one thread"

   def run(self):
      serverProc = self.run_server(threads=1, buffers=1, schedalg="FIFO")
      requests = ["/output.cgi?3", "/3.cgi?1", "/2.cgi?1", "/1.cgi?1"]
      conns = list()
      for request in requests:
         time.sleep(0.1)
         connection = httplib.HTTPConnection("localhost", self.port, timeout=10)
         connection.request("GET", request)
         conns.append(connection)

      clients = [threading.Thread(target=self.client_run, args=(conn,))
            for conn in conns]
      for client in clients:
         client.start()

      for i in range(4):
         if not clients[i].is_alive():
            self.fail("reply " + str(i) + " received too soon")
            continue
         clients[i].join()
         for j in range(i+1, 4):
            if not clients[j].is_alive():
               self.fail("reply " + str(j) + " was received before reply " + str(i))
      serverProc.kill()
      self.done()

class Sff(ServerTest):
   name = "sff"
   description = "Smallest file first with dynamic requests and one thread"

   def run(self):
      serverProc = self.run_server(threads=1, buffers=10, schedalg="SFF")
      conns = [None] * 4

      conns[0] = httplib.HTTPConnection("localhost", self.port, timeout=4)
      conns[0].request("GET", "/output.cgi?3")

      time.sleep(1)

      conns[2] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[2].request("GET", "/2.cgi?1")

      time.sleep(0.1)

      conns[3] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[3].request("GET", "/3.cgi?1")

      time.sleep(0.1)

      conns[1] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[1].request("GET", "/1.cgi?1")

      clients = [threading.Thread(target=self.client_run, args=(conn,)) for conn in conns]

      for client in clients:
         client.start()

      for i in range(4):
         if not clients[i].is_alive():
            self.fail("reply " + str(i) + " received too soon")
            continue
         clients[i].join()
         for j in range(i+1, 4):
            if not clients[j].is_alive():
               self.fail("reply " + str(j) + " was received before reply " + str(i))
      serverProc.kill()
      self.done()


class Sff2(ServerTest):
   name = "sff2"
   description = "Smallest file first with dynamic requests"
   def run(self):
      serverProc = self.run_server(threads=2, buffers=10, schedalg="SFF")
      conns = [None] * 6
      conns[0] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[0].request("GET", "/output.cgi?3")
      conns[1] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[1].request("GET", "/output.cgi?3")
      time.sleep(1)
      conns[3] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[3].request("GET", "/2.cgi?1")
      conns[4] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[4].request("GET", "/2.cgi?1")
      conns[5] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[5].request("GET", "/3.cgi?1")
      conns[2] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[2].request("GET", "/1.cgi?1")
      clients = [threading.Thread(target=self.client_run, args=(conn,)) for conn in conns]
      for client in clients:
         client.start()
      for i in [2, 5]:
         if not clients[i].is_alive():
            self.fail("reply " + str(i) + " received too soon")
            continue
         clients[i].join()
      for client in clients:
         client.join()
      serverProc.kill()
      self.done()

class Sfnf(ServerTest):
   name = "sfnf"
   description = "Shortest file name first with dynamic requests"
   def run(self):
      serverProc = self.run_server(threads=2, buffers=10, schedalg="SFNF")
      conns = [None] * 6
      conns[0] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[0].request("GET", "/output.cgi?3")
      conns[1] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[1].request("GET", "/output.cgi?3")
      time.sleep(1)
      conns[3] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[3].request("GET", "/1_.cgi?1")
      conns[4] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[4].request("GET", "/1_.cgi?1")
      conns[5] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[5].request("GET", "/1__.cgi?1")
      conns[2] = httplib.HTTPConnection("localhost", self.port, timeout=10)
      conns[2].request("GET", "/1.cgi?1")
      clients = [threading.Thread(target=self.client_run, args=(conn,)) for conn in conns]
      for client in clients:
         client.start()
      for i in [2, 5]:
         if not clients[i].is_alive():
            self.fail("reply " + str(i) + " received too soon")
            continue
         clients[i].join()
      for client in clients:
         client.join()
      serverProc.kill()
      self.done()

class Pool(ServerTest):
   name = "pool"
   description = "check if using a fixed size thread pool"
   def run(self):
      serverProc = self.run_server(threads=2, buffers=8, schedalg="FIFO")
      conns = list()

      for i in range(4):
         conns.append(httplib.HTTPConnection("localhost", self.port, timeout=10))
         conns[-1].request("GET", "/output.cgi?1")
         time.sleep(0.1)

      try:
         conns[0].getresponse()
         conns[1].getresponse()
         t1 = time.time()

         conns[2].getresponse()
         conns[3].getresponse()
         t2 = time.time()
         if t2 < t1 + 1:
            self.fail("not using a fixed size thread pool (size 2)")
      except Exception as inst:
         self.fail("Client failed with error: " + str(inst))

      serverProc.kill()
      self.done()


class Pool2(ServerTest):
   name = "pool2"
   description = "check if using a fixed size thread pool"
   def run(self):
      serverProc = self.run_server(threads=3, buffers=8, schedalg="FIFO")
      conns = list()

      for i in range(6):
         conns.append(httplib.HTTPConnection("localhost", self.port, timeout=10))
         conns[-1].request("GET", "/output.cgi?1")
         time.sleep(0.1)

      try:
         conns[0].getresponse()
         conns[1].getresponse()
         conns[2].getresponse()
         t1 = time.time()

         conns[3].getresponse()
         conns[4].getresponse()
         conns[5].getresponse()
         t2 = time.time()
         if t2 < t1 + 1:
            self.fail("not using a fixed size thread pool (size 2)")
      except Exception as inst:
         self.fail("Client failed with error: " + str(inst))

      serverProc.kill()
      self.done()


class Locks(ServerTest):
   name = "locks"
   description = "many concurrent requests to test locking"
   threads = 8
   buffers = 16
   schedalg = "FIFO"
   num_clients = 20
   loops = 20
   requests = ["/home.html", "/output.cgi?0.3"]
   def many_reqs(self):
      for i in range(self.loops):
         for request in self.requests:
            conn = httplib.HTTPConnection("localhost", self.port, timeout=8)
            conn.request("GET", request)
            self.client_run(conn)
   def run(self):
      serverProc = self.run_server(threads=self.threads, buffers=self.buffers, schedalg=self.schedalg)
      clients = [threading.Thread(target=self.many_reqs) for i in range(self.num_clients)]
      for client in clients:
         client.start()
      for client in clients:
         client.join()
      serverProc.kill()
      self.done()

class Locks2(Locks):
   name = "locks2"
   threads = 32
   buffers = 64
   num_clients = 40
   loops = 10
   requests = ["/home.html", "/output.cgi?0.3", "/in", "/output.cgi?0.2"]

class Locks3(Locks):
   name = "locks3"
   threads = 64
   buffers = 128
   schedalg = "SFF"
   num_clients = 50
   loops = 6
   requests = ["/home.html", "/output.cgi?0.3", "/in", "/output.cgi?0.2"]
   timeout = 60

class Locks4(Locks):
   name = "locks4"
   threads = 25
   buffers = 27
   num_clients = 20
   loops = 20
   requests = ["/output.cgi?0.01", "/output.cgi?0.02", "/output.cgi?0.005"]
   timeout = 100

class Fewer(Locks):
   name = "fewer"
   description = "fewer buffers than threads"
   threads = 16
   buffers = 8
   num_clients = 20
   loops = 20

class Fewer2(Locks):
   name = "fewer2"
   description = "fewer buffers than threads"
   threads = 32
   buffers = 1
   num_clients = 20
   loops = 20

class BusyWait(ServerTest):
   name = "nobusywait"
   description = "test to make sure you are not busy-waiting"
   def run(self):
      serverProc = self.run_server(threads=10, buffers=1, schedalg="FIFO")
      time.sleep(5);
      cmd = ['ps', '--no-headers', '-o', 'time', str(serverProc.pid)]
      ps = subprocess.Popen(cmd, stdout=subprocess.PIPE).communicate()[0]

      hr, min, sec = tuple(map(int, ps.split(':')))
      total = sec + (60 * (min + (60 * hr)))
      print 'idle server spent %d seconds running on CPU' % (total,)

      if total != 0:
         self.fail('idle server should spend 0 seconds running on CPU')
      serverProc.kill()
      self.done()

test_list = [Basic, BusyWait, Fifo, Sff, Sff2, Sfnf, Pool, Pool2, Locks, Locks2, Locks3, Locks4, Fewer, Fewer2]

