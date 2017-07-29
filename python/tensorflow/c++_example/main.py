import tensorflow as tf
import numpy as np

with tf.Session() as sess:
    #a = tf.Variable(4.0, name="a")
    #b = tf.Variable(5.0, name="b")
    #c = tf.mul(a, b, name="c")
    #sess.run(tf.initialize_all_variables())

    #tf.train.write_graph(sess.graph_def, 'model/', 'mul_graph.pb', as_text=False)

    #a = tf.Variable(tf.zeros([2,2]), name="a",dtype=tf.float32)
    #a = tf.Variable(initial_value=tf.constant([1,2,3,4], dtype=tf.float32, shape=[2,2]))
    #a = tf.Variable(initial_value=,trainable=,collections=,validate_shape=,caching_device=,name=,variable_def=,dtype=)
    #tf.constant(value,dtype=,shape=,name=)
    a = tf.Variable(initial_value=tf.constant([1.0,2.0,3.0,4.0],dtype=tf.float32,shape=[2,2]),name="a")
    #b = tf.Variable(tf.zeros([2,2]), name="b", dtype=tf.float32)
    b = tf.Variable(initial_value=tf.constant([1.0,2.0,3.0,4.0],dtype=tf.float32, shape=[2,2]),name="b")
    c = tf.matmul(a,b, name="c")

    sess.run(tf.initialize_all_variables())
    tf.train.write_graph(sess.graph_def, "model/", 'matrix_mul.pb', as_text=False)



