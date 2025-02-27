# import numpy as np
# import pandas as pd
# import matplotlib.pyplot as plt
# from sklearn.linear_model import LinearRegression

# file_path = "alfa_exact.csv"  

# data = pd.read_csv(file_path, sep=';', header=None, usecols=[1, 4])
# data.columns = ["X", "Y"]

# X = data["X"].values.reshape(-1, 1)
# Y = data["Y"].values

# model = LinearRegression()
# model.fit(X, Y)

# slope = model.coef_[0]  
# intercept = model.intercept_  

# print(f"Equação da reta: K = {slope:.4f} * N + {intercept:.4f}")

# X_pred = np.linspace(X.min(), X.max(), 100).reshape(-1, 1)
# Y_pred = model.predict(X_pred)

# plt.scatter(X, Y, color='blue', label='Dados')
# plt.plot(X_pred, Y_pred, color='red', label='Regressão Linear')
# plt.xlabel("Vértices (X)")
# plt.ylabel("K Exato (Y)")
# plt.title("Regressão Linear entre (Vértices de G) e (K Exato)")
# plt.legend()
# plt.show()

# import numpy as np
# import pandas as pd
# import matplotlib.pyplot as plt
# from sklearn.linear_model import LinearRegression

# file_path = "input/result-alfa-complete.csv"  

# # Carregar os dados (supondo que colunas 1 e 2 sejam Vértices e Arestas, e coluna 4 seja K Exato)
# data = pd.read_csv(file_path, sep=';', header=None, usecols=[0, 1, 3])
# data.columns = ["Vértices", "Arestas", "K"]

# # Definir variáveis independentes (X) e dependente (Y)
# X = data[["Vértices", "Arestas"]].values  # Duas colunas agora
# Y = data["K"].values  # Variável dependente

# # Criar e treinar o modelo de regressão linear múltipla
# model = LinearRegression()
# model.fit(X, Y)

# # Coeficientes e intercepto
# slope_vertices = model.coef_[0]  # Coeficiente para Vértices
# slope_arestas = model.coef_[1]   # Coeficiente para Arestas
# intercept = model.intercept_  

# # Exibir a equação da reta ajustada
# print(f"Equação da regressão: K = {slope_vertices:.4f} * Vértices + {slope_arestas:.4f} * Arestas + {intercept:.4f}")

# # Gerar previsões para gráfico (mantendo proporções médias para uma das variáveis)
# X_pred = np.linspace(X[:, 0].min(), X[:, 0].max(), 100).reshape(-1, 1)  # Variação de Vértices
# Arestas_medio = np.full((100, 1), data["Arestas"].mean())  # Mantém Arestas fixo na média
# X_pred_full = np.hstack((X_pred, Arestas_medio))  # Combina em matriz 2D

# Y_pred = model.predict(X_pred_full)

# # Gráfico de regressão considerando apenas a variação dos vértices
# plt.scatter(data["Vértices"], data["K"], color='blue', label='Dados')
# plt.plot(X_pred, Y_pred, color='red', label='Regressão Linear Múltipla')
# plt.xlabel("Vértices")
# plt.ylabel("K Exato")
# plt.title("Regressão Linear entre Vértices, Arestas e K Exato")
# plt.legend()
# plt.show()

