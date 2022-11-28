import sqlite3

PATH_DB = 'supplr_db.db'

def init_db(PATH_DB, nodes):
    sqlite_connection = sqlite3.connect(PATH_DB)
    cursor = sqlite_connection.cursor()
    for node in range(1,128):
        sqlite_query = 'DROP TABLE IF EXISTS board_' +str(node)+';'
        cursor.execute(sqlite_query)
    for node in nodes:
        sqlite_query = 'CREATE TABLE IF NOT EXISTS board_' +str(node)+'(channel INTEGER, value REAL);'
        cursor.execute(sqlite_query)
        for ch in range(1,129):
            sqlite_query = 'INSERT INTO board_' +str(node)+'(channel, value) VALUES(?,?);'
            data = (ch, None)
            cursor.execute(sqlite_query, data)
    sqlite_connection.commit()
    sqlite_connection.close()

def update_node_event(PATH_DB, node, channel, value):
    sqlite_connection = sqlite3.connect(PATH_DB)
    cursor = sqlite_connection.cursor()
    sqlite_query = 'UPDATE board_'+str(node)+' SET channel='+str(channel)+', value='+str(value)+' WHERE channel='+str(channel)+';'
    cursor.execute(sqlite_query)
    sqlite_connection.commit()
    sqlite_connection.close()

def delete(PATH_DB, node):
    sqlite_connection = sqlite3.connect(PATH_DB)
    cursor = sqlite_connection.cursor()
    sqlite_query = 'DROP TABLE IF EXISTS board_'+str(node)+';'
    cursor.execute(sqlite_query)
    sqlite_connection.commit()
    sqlite_connection.close()

def dict_factory(cursor, row):
    d = {}
    for idx, col in enumerate(cursor.description):
        d[col[0]] = row[idx]
    return d

def get_node_data(PATH_DB, node):
    sqlite_connection = sqlite3.connect(PATH_DB)
    sqlite_connection.row_factory = dict_factory
    cursor = sqlite_connection.cursor()
    sqlite_query = 'SELECT * FROM board_'+str(node)+';'
    cursor.execute(sqlite_query)
    data = cursor.fetchall()
    sqlite_connection.commit()
    sqlite_connection.close()
    return data
