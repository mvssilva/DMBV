import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression

file_path = "alfa_exact.csv"  

data = pd.read_csv(file_path, sep=';', header=None, usecols=[1, 4])
data.columns = ["X", "Y"]

X = data["X"].values.reshape(-1, 1)
Y = data["Y"].values

model = LinearRegression()
model.fit(X, Y)

slope = model.coef_[0]  
intercept = model.intercept_  

print(f"Equação da reta: K = {slope:.4f} * N + {intercept:.4f}")

X_pred = np.linspace(X.min(), X.max(), 100).reshape(-1, 1)
Y_pred = model.predict(X_pred)

plt.scatter(X, Y, color='blue', label='Dados')
plt.plot(X_pred, Y_pred, color='red', label='Regressão Linear')
plt.xlabel("Vértices (X)")
plt.ylabel("K Exato (Y)")
plt.title("Regressão Linear entre (Vértices de G) e (K Exato)")
plt.legend()
plt.show()