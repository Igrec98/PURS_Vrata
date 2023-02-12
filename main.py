from flask import Flask, render_template, request, redirect, url_for, session
from hashlib import sha256
from flask_mysqldb import MySQL
import requests


app = Flask(__name__)
app.secret_key = '_5#y2L"F4Q8z\n\xec]/'

app.config['MYSQL_HOST'] = 'localhost'
app.config['MYSQL_USER'] = 'root'
app.config['MYSQL_PASSWORD'] ='Matej2612!'
app.config['MYSQL_DB'] = "Vrata"
mysql = MySQL(app)

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'GET':
        return render_template('login.html')
    elif request.method == 'POST':
        email = request.form.get('email')
        password = sha256(request.form.get('password').encode()).hexdigest()

        query = f"SELECT username FROM Korisnici WHERE HEX(password) = '{password}' AND email = '{email}'"
        cursor = mysql.connection.cursor()
        cursor.execute(query)
        korisnik = cursor.fetchall()
        if korisnik:
            session['username'] = korisnik[0][0]
            return redirect(url_for('pocetna')), 303

        else:
            return render_template('login.html', error='Uneseni su krivi korisnički podaci')


@app.route('/', methods=['GET'])
def pocetna():
    print(session)

    if 'username' in session:
        return render_template('index.html')

    return redirect(url_for('login')), 303


@app.route('/otvaraj', methods=['GET'])
def otvarajVrata():
    query = f"INSERT INTO unos (username, stanje) VALUES ('{session['username']}','otvoreno');"
    cursor = mysql.connection.cursor()
    cursor.execute(query)
    mysql.connection.commit()

    response = requests.get('http://192.168.100.16/otvaraj')
    return response.json()

@app.route('/zatvaraj', methods=['GET'])
def zatvarajVrata():
    query = f"INSERT INTO unos (username, stanje) VALUES ('{session['username']}','zatvoreno');"
    cursor = mysql.connection.cursor()
    cursor.execute(query)
    mysql.connection.commit()

    response = requests.get('http://192.168.100.16/zatvaraj')
    return response.json()


if __name__ == '__main__':
    app.run()