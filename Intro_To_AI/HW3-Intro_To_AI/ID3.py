import math

from DecisonTree import Leaf, Question, DecisionNode, class_counts
from utils import *

"""
Make the imports of python packages needed
"""


class ID3:
    def __init__(self, label_names: list,  target_attribute='diagnosis'):
        self.label_names = label_names
        self.target_attribute = target_attribute
        self.tree_root = None
        self.used_features = set()

    @staticmethod
    def entropy(rows: np.ndarray, labels: np.ndarray):
        """
        Calculate the entropy of a distribution for the classes probability values.
        :param rows: array of samples
        :param labels: rows data labels.
        :return: entropy value.
        """
        # TODO:
        #  Calculate the entropy of the data as shown in the class.
        #  - You can use counts as a helper dictionary of label -> count, or implement something else.

        counts = class_counts(rows, labels)
        impurity = 0.0

        # ====== YOUR CODE: ======
        total = float(len(labels))
        if total == 0:
            return 0.0

        for count in counts.values():
            p = count / total
            impurity -= p * math.log2(p)
        # ========================

        return impurity

    def info_gain(self, left, left_labels, right, right_labels, current_uncertainty):
        """
        Calculate the information gain, as the uncertainty of the starting node, minus the weighted impurity of
        two child nodes.
        :param left: the left child rows.
        :param left_labels: the left child labels.
        :param right: the right child rows.
        :param right_labels: the right child labels.
        :param current_uncertainty: the current uncertainty of the current node
        :return: the info gain for splitting the current node into the two children left and right.
        """
        # TODO:
        #  - Calculate the entropy of the data of the left and the right child.
        #  - Calculate the info gain as shown in class.
        assert (len(left) == len(left_labels)) and (len(right) == len(right_labels)), \
            'The split of current node is not right, rows size should be equal to labels size.'

        info_gain_value = 0.0
        # ====== YOUR CODE: ======
        left_entropy = self.entropy(left, left_labels)
        right_entropy = self.entropy(right, right_labels)
        n_left = len(left_labels)
        n_right = len(right_labels)
        n_total = n_left + n_right

        if n_total == 0:
            return 0.0

        p_left = n_left / n_total
        p_right = n_right / n_total

        info_gain_value = current_uncertainty - p_left * left_entropy - p_right * right_entropy
        # ========================

        return info_gain_value

    def partition(self, rows, labels, question: Question, current_uncertainty):
        """
        Partitions the rows by the question.
        :param rows: array of samples
        :param labels: rows data labels.
        :param question: an instance of the Question which we will use to partition the data.
        :param current_uncertainty: the current uncertainty of the current node
        :return: Tuple of (gain, true_rows, true_labels, false_rows, false_labels)
        """
        # TODO:
        #   - For each row in the dataset, check if it matches the question.
        #   - If so, add it to 'true rows', otherwise, add it to 'false rows'.
        #   - Calculate the info gain using the `info_gain` method.

        gain, true_rows, true_labels, false_rows, false_labels = None, None, None, None, None
        assert len(rows) == len(labels), 'Rows size should be equal to labels size.'

        # ====== YOUR CODE: ======
        for idx, row in enumerate(rows):
            answer = question.match(row)
            if answer:
                if true_rows is None:
                    true_rows = []
                    true_labels = []
                true_rows.append(row)
                true_labels.append(labels[idx])
            else:
                if false_rows is None:
                    false_rows = []
                    false_labels = []
                false_rows.append(row)
                false_labels.append(labels[idx])

        true_rows = np.asarray(true_rows) if true_rows is not None else None
        false_rows = np.asarray(false_rows) if false_rows is not None else None
        true_labels = np.asarray(true_labels) if true_labels is not None else None
        false_labels = np.asarray(false_labels) if false_labels is not None else None

        if(true_rows is None or false_rows is None or true_labels is None or false_labels is None):
            return 0.0, true_rows, true_labels, false_rows, false_labels
        else:
            gain = self.info_gain(true_rows, true_labels, false_rows, false_labels, current_uncertainty)
        # ========================

        return gain, true_rows, true_labels, false_rows, false_labels

    def find_best_split(self, rows, labels):
        """
        Find the best question to ask by iterating over every feature / value and calculating the information gain.
        :param rows: array of samples
        :param labels: rows data labels.
        :return: Tuple of (best_gain, best_question, best_true_rows, best_true_labels, best_false_rows, best_false_labels)
        """
        # TODO:
        #   - For each feature of the dataset, build a proper question to partition the dataset using this feature.
        #   - find the best feature to split the data. (using the `partition` method)
        best_gain = - math.inf  # keep track of the best information gain
        best_question = None  # keep train of the feature / value that produced it
        best_false_rows, best_false_labels = None, None
        best_true_rows, best_true_labels = None, None
        current_uncertainty = self.entropy(rows, labels)

        # ====== YOUR CODE: ======
        features_num = rows.shape[1]

        for feature_idx in range(features_num):
            if feature_idx in self.used_features:
                continue

            values = np.unique(rows[:, feature_idx])
            thresholds = []
            for i,value in enumerate(values):
                if i == len(values) - 1:
                    break
                thresholds.append((value + values[i+1]) / 2.0)

            for thresh in thresholds:
                feature_name = self.label_names[feature_idx] if self.label_names is not None else str(feature_idx)
                question = Question(feature_name, feature_idx, thresh)

                gain, true_rows, true_labels, false_rows, false_labels = self.partition(rows, labels, question, current_uncertainty)

                if true_rows is None or false_rows is None or true_labels is None or false_labels is None:
                    continue

                if gain >= best_gain:
                    best_gain = gain
                    best_question = question
                    best_true_rows, best_true_labels = true_rows, true_labels
                    best_false_rows, best_false_labels = false_rows, false_labels
        # ========================

        return best_gain, best_question, best_true_rows, best_true_labels, best_false_rows, best_false_labels

    def build_tree(self, rows, labels):
        """
        Build the decision Tree in recursion.
        :param rows: array of samples
        :param labels: rows data labels.
        :return: a Question node, This records the best feature / value to ask at this point, depending on the answer. 

v        """
        # TODO:
        #   - Try partitioning the dataset using the feature that produces the highest gain.
        #   - Recursively build the true, false branches.
        #   - Build the Question node which contains the best question with true_branch, false_branch as children
        best_question = None
        true_branch, false_branch = None, None

        # ====== YOUR CODE: ======
        best_gain, best_question, true_rows, true_labels, false_rows, false_labels = self.find_best_split(rows, labels)

        if best_question is None or best_gain <= 0 or true_rows is None or false_rows is None:
            return Leaf(rows, labels)

        feature_idx = best_question.column
        not_used = feature_idx not in self.used_features
        self.used_features.add(feature_idx)

        true_branch = self.build_tree(true_rows, true_labels)
        false_branch = self.build_tree(false_rows, false_labels)

        if not_used:
            self.used_features.remove(feature_idx)
        # ========================

        return DecisionNode(best_question, true_branch, false_branch)

    def fit(self, x_train, y_train):
        """
        Trains the ID3 model. By building the tree.
        :param x_train: A labeled training data.
        :param y_train: training data labels.
        """
        # TODO: Build the tree that fits the input data and save the root to self.tree_root

        # ====== YOUR CODE: ======
        x_train = np.asarray(x_train)
        y_train = np.asarray(y_train)

        if x_train.ndim != 2:
            raise ValueError("x_train must be a 2D array of shape (N, D).")
        if y_train.ndim != 1:
            raise ValueError("y_train must be a 1D array of shape (N,).")
        if len(x_train) != len(y_train):
            raise ValueError("x_train and y_train must have the same number of samples.")

        self.used_features = set()
        self.tree_root = self.build_tree(x_train, y_train)
        # ========================

    def predict_sample(self, row, node: DecisionNode or Leaf = None):
        """
        Predict the most likely class for single sample in subtree of the given node.
        :param row: vector of shape (1,D).
        :return: The row prediction.
        """
        # TODO: Implement ID3 class prediction for set of data.
        #   - Decide whether to follow the true-branch or the false-branch.
        #   - Compare the feature / value stored in the node, to the example we're considering.

        if node is None:
            node = self.tree_root
        prediction = None

        # ====== YOUR CODE: ======
        row = np.asarray(row)
        if row.ndim == 2:
            if row.shape[0] != 1:
                raise ValueError("row must be shape (D,) or (1, D).")
            row = row[0]
        elif row.ndim != 1:
            raise ValueError("row must be shape (D,) or (1, D).")

        if isinstance(node, Leaf):
            return max(node.predictions, key=node.predictions.get)

        if node.question.match(row):
            prediction = self.predict_sample(row, node.true_branch)
        else:
            prediction = self.predict_sample(row, node.false_branch)
        # ========================

        return prediction

    def predict(self, rows):
        """
        Predict the most likely class for each sample in a given vector.
        :param rows: vector of shape (N,D) where N is the number of samples.
        :return: A vector of shape (N,) containing the predicted classes.
        """
        # TODO:
        #  Implement ID3 class prediction for set of data.

        y_pred = None

        # ====== YOUR CODE: ======
        rows = np.asarray(rows)
        if rows.ndim != 2:
            raise ValueError("x_test must be a 2D array of shape (N, D).")

        predictions = [self.predict_sample(row) for row in rows]
        y_pred = np.array(predictions)
        # ========================

        return y_pred
