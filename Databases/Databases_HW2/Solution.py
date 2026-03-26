from typing import List, Tuple
from psycopg2 import sql
from datetime import date, datetime
import Utility.DBConnector as Connector
from Utility.ReturnValue import ReturnValue
from Utility.Exceptions import DatabaseException
from Business.Customer import Customer, BadCustomer
from Business.Order import Order, BadOrder
from Business.Dish import Dish, BadDish
from Business.OrderDish import OrderDish


# ---------------------------------- CRUD API: ----------------------------------
# Basic database functions


def create_tables() -> None:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
            CREATE TABLE Customers (
                cust_id integer NOT NULL,
                full_name text NOT NULL,
                age integer NOT NULL,
                phone text NOT NULL,
                CONSTRAINT customer_pkey PRIMARY KEY (cust_id),
                CONSTRAINT CUST_ID_IS_POSITIVE CHECK (cust_id > 0),
                CONSTRAINT LEGAL_AGE CHECK (age >= 18 AND age <= 120),
                CONSTRAINT LEGAL_PHONE CHECK (length(phone) = 10)
            );
            CREATE TABLE Orders (
                order_id integer NOT NULL,
                order_date timestamp(0) without time zone NOT NULL,
                delivery_fee numeric NOT NULL,
                delivery_address text NOT NULL,
                CONSTRAINT order_pkey PRIMARY KEY (order_id),
                CONSTRAINT ORDER_ID_IS_POSITIVE CHECK (order_id > 0),
                CONSTRAINT DELIVERY_FEE_NON_NEGATIVE CHECK (delivery_fee >= 0),
                CONSTRAINT LEGAL_DELIVERY_ADDRESS CHECK (length(delivery_address) >= 5)
            );
            CREATE TABLE Dishes (
                dish_id integer NOT NULL,
                name text NOT NULL,
                price numeric NOT NULL,
                is_active boolean NOT NULL,
                CONSTRAINT dish_pkey PRIMARY KEY (dish_id),
                CONSTRAINT DISH_ID_IS_POSITIVE CHECK (dish_id > 0),
                CONSTRAINT PRICE_IS_POSITIVE CHECK (price > 0),
                CONSTRAINT LEGAL_NAME CHECK (length(name) >= 4)
            );
            CREATE TABLE Order_Placement (
                order_id integer NOT NULL,
                cust_id  integer NOT NULL,
                CONSTRAINT op_pkey PRIMARY KEY (order_id),
                CONSTRAINT fk_order FOREIGN KEY (order_id) REFERENCES Orders(order_id) ON DELETE CASCADE,
                CONSTRAINT fk_customer FOREIGN KEY (cust_id) REFERENCES Customers(cust_id) ON DELETE CASCADE
            );
            CREATE TABLE Order_Dishes (
                order_id integer NOT NULL,
                dish_id integer NOT NULL,
                amount integer NOT NULL,
                orig_price numeric NOT NULL,
                CONSTRAINT order_dishes_pkey PRIMARY KEY (order_id, dish_id),
                CONSTRAINT AMOUNT_IS_POSITIVE CHECK (amount >= 0),
                CONSTRAINT fk_order_dishes_order FOREIGN KEY (order_id) REFERENCES orders(order_id) ON DELETE CASCADE,
                CONSTRAINT fk_order_dishes_dish FOREIGN KEY (dish_id) REFERENCES Dishes(dish_id) ON DELETE CASCADE
            );
            CREATE TABLE Customers_Dishes_Rate (
                cust_id integer NOT NULL,
                dish_id integer NOT NULL,
                rate integer NOT NULL,
                CONSTRAINT customers_dishes_rate_pkey PRIMARY KEY (cust_id, dish_id),
                CONSTRAINT RATE_BETWEEN_1_TO_5 CHECK (rate BETWEEN 1 AND 5),
                CONSTRAINT fk_customers_dishes_rate_cust FOREIGN KEY (cust_id) REFERENCES Customers(cust_id) ON DELETE CASCADE,
                CONSTRAINT fk_customers_dishes_rate_dish FOREIGN KEY (dish_id) REFERENCES Dishes(dish_id) ON DELETE CASCADE
            );
        """)
        conn.execute(query)
        create_views(conn)
    except Exception:
        pass
    finally:
        conn.close()


def clear_tables() -> None:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
                    DELETE FROM Order_Dishes;
                    DELETE FROM Order_Placement;
                    DELETE FROM Orders;
                    DELETE FROM Customers;
                    DELETE FROM Dishes;
                    DELETE FROM Customers_Dishes_Rate;
                """)
        conn.execute(query)
    except Exception:
        pass
    finally:
        conn.close()


def drop_tables() -> None:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
                DROP TABLE IF EXISTS Order_Dishes CASCADE;
                DROP TABLE IF EXISTS Order_Placement CASCADE;
                DROP TABLE IF EXISTS Orders CASCADE;
                DROP TABLE IF EXISTS Customers CASCADE;
                DROP TABLE IF EXISTS Dishes CASCADE;
                DROP TABLE IF EXISTS Customers_Dishes_Rate CASCADE;
            """)
        conn.execute(query)
    except Exception:
        pass
    finally:
        conn.close()


# CRUD API

def add_customer(customer: Customer) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("INSERT INTO Customers(cust_id, full_name, age, phone) VALUES({id}, {name}, {age}, {phone})").format(
            id=sql.Literal(customer.get_cust_id()),
            name=sql.Literal(customer.get_full_name()),
            age=sql.Literal(customer.get_age()),
            phone=sql.Literal(customer.get_phone())
        )
        conn.execute(query)
        return ReturnValue.OK
    except DatabaseException.NOT_NULL_VIOLATION:
        return ReturnValue.BAD_PARAMS
    except DatabaseException.CHECK_VIOLATION:
        return ReturnValue.BAD_PARAMS
    except DatabaseException.UNIQUE_VIOLATION:
        return ReturnValue.ALREADY_EXISTS
    except Exception:
        return ReturnValue.ERROR
    finally:
        conn.close()


def get_customer(customer_id: int) -> Customer:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("SELECT * FROM Customers WHERE cust_id = {0}").format(sql.Literal(customer_id))
        rows_effected, result = conn.execute(query)
        if rows_effected == 0:
            return BadCustomer()
        return Customer(result[0]['cust_id'], result[0]['full_name'], result[0]['age'], result[0]['phone'])
    except Exception:
        return BadCustomer()
    finally:
        conn.close()


def delete_customer(customer_id: int) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("DELETE FROM Customers WHERE cust_id={0}").format(sql.Literal(customer_id))
        rows_effected, _ = conn.execute(query)
        if rows_effected == 0:
            return ReturnValue.NOT_EXISTS
        return ReturnValue.OK
    except DatabaseException.ConnectionInvalid:
        return ReturnValue.ERROR
    except Exception:
        return ReturnValue.NOT_EXISTS
    finally:
        conn.close()


def add_order(order: Order) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL(
            "INSERT INTO Orders(order_id, order_date, delivery_fee, delivery_address) VALUES({id}, {datetime}, {fee}, {address})").format(
            id=sql.Literal(order.get_order_id()),
            datetime=sql.Literal(order.get_datetime()),
            fee=sql.Literal(order.get_delivery_fee()),
            address=sql.Literal(order.get_delivery_address())
        )
        conn.execute(query)
        return ReturnValue.OK
    except DatabaseException.NOT_NULL_VIOLATION:
        return ReturnValue.BAD_PARAMS
    except DatabaseException.CHECK_VIOLATION:
        return ReturnValue.BAD_PARAMS
    except DatabaseException.UNIQUE_VIOLATION:
        return ReturnValue.ALREADY_EXISTS
    except Exception:
        return ReturnValue.ERROR
    finally:
        conn.close()


def get_order(order_id: int) -> Order:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("SELECT * FROM orders WHERE order_id = {0}").format(sql.Literal(order_id))
        rows_effected, result = conn.execute(query)
        if rows_effected == 0:
            return BadOrder()
        return Order(result[0]['order_id'], result[0]['order_date'], result[0]['delivery_fee'], result[0]['delivery_address'])
    except Exception:
        return BadOrder()
    finally:
        conn.close()


def delete_order(order_id: int) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("DELETE FROM Orders WHERE order_id={0}").format(sql.Literal(order_id))
        rows_effected, _ = conn.execute(query)
        if rows_effected == 0:
            return ReturnValue.NOT_EXISTS
        return ReturnValue.OK
    except DatabaseException.ConnectionInvalid:
        return ReturnValue.ERROR
    except Exception:
        return ReturnValue.NOT_EXISTS
    finally:
        conn.close()


def add_dish(dish: Dish) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL(
            "INSERT INTO Dishes(dish_id, name, price, is_active) VALUES({id}, {name}, {price}, {active})").format(
            id=sql.Literal(dish.get_dish_id()),
            name=sql.Literal(dish.get_name()),
            price=sql.Literal(dish.get_price()),
            active=sql.Literal(dish.get_is_active())
        )
        conn.execute(query)
        return ReturnValue.OK
    except DatabaseException.NOT_NULL_VIOLATION:
        return ReturnValue.BAD_PARAMS
    except DatabaseException.CHECK_VIOLATION:
        return ReturnValue.BAD_PARAMS
    except DatabaseException.UNIQUE_VIOLATION:
        return ReturnValue.ALREADY_EXISTS
    except Exception:
        return ReturnValue.ERROR
    finally:
        conn.close()


def get_dish(dish_id: int) -> Dish:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("SELECT * FROM Dishes WHERE dish_id = {0}").format(sql.Literal(dish_id))
        rows_effected, result = conn.execute(query)
        if rows_effected == 0:
            return BadDish()
        return Dish(result[0]['dish_id'], result[0]['name'], result[0]['price'], result[0]['is_active'])
    except Exception:
        return BadDish()
    finally:
        conn.close()


def update_dish_price(dish_id: int, price: float) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("UPDATE Dishes SET price = {0} WHERE dish_id = {1} AND is_active = TRUE").format(
            sql.Literal(price), sql.Literal(dish_id))
        rows_effected, _ = conn.execute(query)
        if rows_effected == 0:
            return ReturnValue.NOT_EXISTS
        return ReturnValue.OK
    except DatabaseException.ConnectionInvalid:
        return ReturnValue.ERROR
    except DatabaseException.NOT_NULL_VIOLATION:
        return ReturnValue.BAD_PARAMS
    except DatabaseException.CHECK_VIOLATION:
        return ReturnValue.BAD_PARAMS
    except Exception:
        return ReturnValue.NOT_EXISTS
    finally:
        conn.close()


def update_dish_active_status(dish_id: int, is_active: bool) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("UPDATE Dishes SET is_active = {0} WHERE dish_id = {1}").format(
            sql.Literal(is_active), sql.Literal(dish_id))
        rows_effected, _ = conn.execute(query)
        if rows_effected == 0:
            return ReturnValue.NOT_EXISTS
        return ReturnValue.OK
    except DatabaseException.ConnectionInvalid:
        return ReturnValue.ERROR
    except Exception:
        return ReturnValue.NOT_EXISTS
    finally:
        conn.close()

def customer_placed_order(customer_id: int, order_id: int) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("INSERT INTO Order_Placement(order_id, cust_id) VALUES({o_id}, {c_id})").format(
            o_id=sql.Literal(order_id),
            c_id=sql.Literal(customer_id)
        )
        conn.execute(query)
        return ReturnValue.OK
    except DatabaseException.ConnectionInvalid:
        return ReturnValue.ERROR
    except DatabaseException.UNIQUE_VIOLATION:
        return ReturnValue.ALREADY_EXISTS
    except DatabaseException.FOREIGN_KEY_VIOLATION:
        return ReturnValue.NOT_EXISTS
    except Exception:
        return ReturnValue.NOT_EXISTS
    finally:
        conn.close()

def get_customer_that_placed_order(order_id: int) -> Customer:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
                    SELECT *
                    FROM Customers AS C
                    WHERE C.cust_id = (SELECT cust_id FROM Order_Placement WHERE order_id = {0})
                """).format(sql.Literal(order_id))
        rows_effected, result = conn.execute(query)
        if rows_effected == 0:
            return BadCustomer()
        return Customer(result[0]['cust_id'], result[0]['full_name'], result[0]['age'], result[0]['phone'])
    except Exception:
        return BadCustomer()
    finally:
        conn.close()

def order_contains_dish(order_id: int, dish_id: int, amount: int) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
            INSERT INTO Order_Dishes (order_id, dish_id, amount, orig_price)
            SELECT {o_id}, dish_id, {amt}, price
            FROM Dishes
            WHERE dish_id = {d_id} AND is_active = TRUE AND EXISTS 
            (SELECT * FROM Orders WHERE order_id = {o_id})
        """).format(
            o_id=sql.Literal(order_id),
            d_id=sql.Literal(dish_id),
            amt=sql.Literal(amount)
        )
        rows_effected, _ = conn.execute(query)
        if rows_effected == 0:
            return ReturnValue.NOT_EXISTS
        return ReturnValue.OK
    except DatabaseException.ConnectionInvalid:
        return ReturnValue.ERROR
    except DatabaseException.UNIQUE_VIOLATION:
        return ReturnValue.ALREADY_EXISTS
    except DatabaseException.FOREIGN_KEY_VIOLATION:
        return ReturnValue.NOT_EXISTS
    except Exception:
        return ReturnValue.BAD_PARAMS
    finally:
        conn.close()


def order_does_not_contain_dish(order_id: int, dish_id: int) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
                    DELETE FROM Order_Dishes
                    WHERE order_id = {o_id} AND dish_id = {d_id}
                """).format(
            o_id=sql.Literal(order_id),
            d_id=sql.Literal(dish_id)
        )
        rows_effected, _ = conn.execute(query)
        if rows_effected == 0:
            return ReturnValue.NOT_EXISTS
        return ReturnValue.OK
    except DatabaseException.ConnectionInvalid:
        return ReturnValue.ERROR
    except Exception:
        return ReturnValue.ERROR
    finally:
        conn.close()


def get_all_order_items(order_id: int) -> List[OrderDish]:
    conn = None
    order_items = []
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
            SELECT dish_id, amount, orig_price
            FROM Order_Dishes 
            WHERE order_id = {0} 
            ORDER BY dish_id ASC
            """).format(sql.Literal(order_id))
        _, result = conn.execute(query)
        for row in result:
            item = OrderDish(row['dish_id'], row['amount'], row['orig_price'])
            order_items.append(item)
    except Exception:
        return []
    finally:
        conn.close()
        return order_items

def customer_rated_dish(cust_id: int, dish_id: int, rating: int) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
        INSERT INTO Customers_Dishes_Rate (cust_id, dish_id, rate)
        VALUES ({c_id}, {d_id}, {r});
        """).format(
            c_id=sql.Literal(cust_id),
            d_id=sql.Literal(dish_id),
            r=sql.Literal(rating)
        )
        conn.execute(query)
        return ReturnValue.OK
    except DatabaseException.ConnectionInvalid:
        return ReturnValue.ERROR
    except DatabaseException.CHECK_VIOLATION:
        return ReturnValue.BAD_PARAMS
    except DatabaseException.UNIQUE_VIOLATION:
        return ReturnValue.ALREADY_EXISTS
    except DatabaseException.FOREIGN_KEY_VIOLATION:
        return ReturnValue.NOT_EXISTS
    except Exception:
        pass
        return ReturnValue.NOT_EXISTS
    finally:
        conn.close()


def customer_deleted_rating_on_dish(cust_id: int, dish_id: int) -> ReturnValue:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("DELETE FROM Customers_Dishes_Rate WHERE cust_id = {c_id} AND dish_id = {d_id}").format(
            c_id=sql.Literal(cust_id),
            d_id=sql.Literal(dish_id)
        )
        rows_effected, _ = conn.execute(query)
        if rows_effected == 0:
            return ReturnValue.NOT_EXISTS
        return ReturnValue.OK
    except Exception:
        return ReturnValue.ERROR
    finally:
        conn.close()

def get_all_customer_ratings(cust_id: int) -> List[Tuple[int, int]]:
    conn = None
    customer_ratings = []
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
                SELECT dish_id, rate
                FROM Customers_Dishes_Rate
                WHERE cust_id = {0} 
                ORDER BY dish_id ASC
                """).format(sql.Literal(cust_id))
        _, result = conn.execute(query)
        for row in result:
            item = (row['dish_id'], row['rate'])
            customer_ratings.append(item)
    except Exception:
        return []
    finally:
        conn.close()
        return customer_ratings
# ---------------------------------- BASIC API: ----------------------------------

# Basic API


def get_order_total_price(order_id: int) -> float:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("SELECT total_price FROM view_total_price_table WHERE order_id = {0}").format(
            sql.Literal(order_id)
        )
        rows_effected, result = conn.execute(query)
        if rows_effected == 0:
            return 0.0
        return float(result[0]['total_price'])
    except Exception:
        return 0.0
    finally:
        conn.close()


def get_customers_spent_max_avg_amount_money() -> List[int]:
    conn = None
    customer_avgs = []
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
                    SELECT cust_id
                    FROM view_avg_price_per_cust
                    WHERE avg_price = (SELECT MAX(avg_price) FROM view_avg_price_per_cust)
                    ORDER BY cust_id ASC
                    """)
        _, result = conn.execute(query)
        for row in result:
            customer_avgs.append(row['cust_id'])
    except Exception:
        return []
    finally:
        conn.close()
        return customer_avgs


def get_most_ordered_dish_in_period(start: datetime, end: datetime) -> Dish:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
            SELECT D.dish_id, D.name, D.price, D.is_active
            FROM view_dish_order_details V
            JOIN Dishes D ON V.dish_id = D.dish_id
            WHERE V.order_date >= {start} AND V.order_date <= {end}
            GROUP BY D.dish_id, D.name, D.price, D.is_active
            ORDER BY SUM(V.amount) DESC, D.dish_id ASC
            LIMIT 1
        """).format(
            start=sql.Literal(start),
            end=sql.Literal(end)
        )
        rows_effected, result = conn.execute(query)
        if result.isEmpty():
            return BadDish()
        return Dish(result[0]['dish_id'], result[0]['name'], result[0]['price'], result[0]['is_active'])
    except Exception:
        return BadDish()
    finally:
        conn.close()


def did_customer_order_top_rated_dishes(cust_id: int) -> bool:
    conn = None
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
        SELECT *
        FROM Order_Placement AS OP
        JOIN Order_Dishes AS OD ON OP.order_id = OD.order_id
        WHERE OP.cust_id = {0} AND OD.dish_id IN (SELECT dish_id FROM view_5_highest_rated_dishes)
        """).format(sql.Literal(cust_id))
        rows_effected, _ = conn.execute(query)
        if rows_effected == 0:
            return False
        return True
    except Exception:
        return False
    finally:
        conn.close()


# ---------------------------------- ADVANCED API: ----------------------------------

# Advanced API


def get_customers_rated_but_not_ordered() -> List[int]:
    conn = None
    non_ordered = []
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
                SELECT DISTINCT CDR.cust_id
                FROM Customers_Dishes_Rate CDR
                WHERE CDR.rate < 3
                AND CDR.dish_id IN (SELECT dish_id FROM view_5_lowest_rated_dishes)
                AND NOT EXISTS (
                    SELECT * 
                    FROM Order_Placement OP
                    JOIN Order_Dishes OD ON OP.order_id = OD.order_id
                    WHERE OP.cust_id = CDR.cust_id 
                    AND OD.dish_id = CDR.dish_id
                )
                ORDER BY CDR.cust_id ASC
        """)
        _, result = conn.execute(query)
        for row in result:
            non_ordered.append(row['cust_id'])
    except Exception:
        return []
    finally:
        conn.close()
        return non_ordered

def get_non_worth_price_increase() -> List[int]:
    conn = None
    non_worth_dishes = []
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
            SELECT DISTINCT D.dish_id
            FROM Dishes AS D
            JOIN view_dish_price_avg_profit AS CurrentStats
            ON D.dish_id = CurrentStats.dish_id AND D.price = CurrentStats.orig_price
            JOIN view_dish_price_avg_profit AS PastStats
            ON D.dish_id = PastStats.dish_id AND PastStats.orig_price < D.price
            WHERE D.is_active = TRUE AND CurrentStats.avg_profit < PastStats.avg_profit
            ORDER BY D.dish_id ASC
            """)
        _, result = conn.execute(query)
        for row in result:
            non_worth_dishes.append(row['dish_id'])
    except Exception:
        pass
    finally:
        conn.close()
        return non_worth_dishes


def get_cumulative_profit_per_month(year: int) -> List[Tuple[int, float]]:
    conn = None
    month_profit_tuples = []
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
            SELECT vm.m AS month,
                COALESCE((
                    SELECT SUM(mp.monthly_profit)
                    FROM view_monthly_profits mp
                    WHERE mp.year = {year} AND mp.month <= vm.m
                ), 0.0) AS cumulative_profit
            FROM view_months vm
            ORDER BY month DESC
        """).format(year=sql.Literal(year))
        _, result = conn.execute(query)
        for row in result:
            month_profit_tuples.append((int(row['month']), float(row['cumulative_profit'])))
    except Exception:
        return []
    finally:
        return month_profit_tuples
        conn.close()


def get_potential_dish_recommendations(cust_id: int) -> List[int]:
    conn = None
    recommended_dishes = []
    try:
        conn = Connector.DBConnector()
        query = sql.SQL("""
                WITH RECURSIVE SimilarCustomers AS(
                SELECT cust_id
                FROM Customers_Dishes_Rate
                WHERE cust_id = {0}
                UNION
                SELECT R_Other.cust_id
                FROM SimilarCustomers AS SC
                JOIN Customers_Dishes_Rate AS R_Current ON SC.cust_id = R_Current.cust_id AND R_Current.rate >= 4
                JOIN Customers_Dishes_Rate AS R_Other ON R_Current.dish_id = R_Other.dish_id AND R_Other.rate >= 4
                )
                SELECT DISTINCT CDR.dish_id
                FROM Customers_Dishes_Rate AS CDR
                JOIN SimilarCustomers AS SC ON CDR.cust_id = SC.cust_id
                JOIN Dishes AS D ON CDR.dish_id = D.dish_id
                WHERE CDR.rate >=4 AND CDR.dish_id NOT IN(
                SELECT OD.dish_id
                FROM Order_Placement AS OP
                JOIN Order_Dishes AS OD ON OP.order_id = OD.order_id
                WHERE OP.cust_id = {0}
                )
                ORDER BY CDR.dish_id ASC
            """).format(sql.Literal(cust_id))
        _, result = conn.execute(query)
        for row in result:
            recommended_dishes.append(row['dish_id'])
    except Exception:
        pass
    finally:
        conn.close()
        return recommended_dishes

# ---------------------------------- VIEWS: ----------------------------------

# Views

def create_views(conn):
    create_view_total_price_table(conn)
    create_view_avg_price_per_cust(conn)
    create_view_all_dish_ratings(conn)
    create_view_5_highest_rated_dishes(conn)
    create_view_5_lowest_rated_dishes(conn)
    create_view_dish_price_avg_profit(conn)
    create_view_dish_order_details(conn)
    create_view_monthly_profits(conn)
    create_view_months(conn)


def create_view_total_price_table(conn):
    query = sql.SQL("""
        CREATE VIEW view_total_price_table AS
        SELECT O.order_id, OP.cust_id, O.order_date, O.delivery_fee + COALESCE(SUM(OD.orig_price * OD.amount), 0) AS total_price
        FROM Orders AS O
        LEFT JOIN Order_Dishes AS OD ON O.order_id = OD.order_id
        LEFT JOIN Order_Placement AS OP ON O.order_id = OP.order_id
        GROUP BY O.order_id, OP.cust_id, O.delivery_fee, O.order_date
        """)
    conn.execute(query)


def create_view_avg_price_per_cust(conn):
    query = sql.SQL("""
                    CREATE VIEW view_avg_price_per_cust AS
                    SELECT cust_id, AVG(total_price) AS avg_price
                    FROM view_total_price_table
                    WHERE cust_id IS NOT NULL
                    GROUP BY cust_id
            """)
    conn.execute(query)

def create_view_all_dish_ratings(conn):
    query = sql.SQL("""
        CREATE VIEW view_all_dish_ratings AS
        SELECT D.dish_id, COALESCE(AVG(CDR.rate), 3) AS avg_rate
        FROM Dishes AS D 
        LEFT JOIN Customers_Dishes_Rate AS CDR ON D.dish_id = CDR.dish_id
        GROUP BY D.dish_id
    """)
    conn.execute(query)

def create_view_5_highest_rated_dishes(conn):
    query = sql.SQL("""
                    CREATE VIEW view_5_highest_rated_dishes AS
                    SELECT dish_id
                    FROM view_all_dish_ratings
                    ORDER BY avg_rate DESC, dish_id ASC
                    LIMIT 5
            """)
    conn.execute(query)

def create_view_5_lowest_rated_dishes(conn):
    query = sql.SQL("""
        CREATE VIEW view_5_lowest_rated_dishes AS
        SELECT dish_id
        FROM view_all_dish_ratings
        ORDER BY avg_rate ASC, dish_id ASC
        LIMIT 5
    """)
    conn.execute(query)


def create_view_dish_price_avg_profit(conn):
    query = sql.SQL("""
        CREATE VIEW view_dish_price_avg_profit AS
        SELECT dish_id, orig_price, AVG(amount) * orig_price AS avg_profit
        FROM Order_Dishes
        GROUP BY dish_id, orig_price
                """)
    conn.execute(query)

def create_view_dish_order_details(conn):
    query = sql.SQL("""
        CREATE VIEW view_dish_order_details AS
        SELECT OD.dish_id, OD.amount, O.order_date
        FROM Order_Dishes AS OD
        JOIN Orders AS O ON OD.order_id = O.order_id
    """)
    conn.execute(query)

def create_view_monthly_profits(conn):
    query = sql.SQL("""
        CREATE VIEW view_monthly_profits AS
        SELECT 
            EXTRACT(YEAR FROM order_date) AS year,
            EXTRACT(MONTH FROM order_date) AS month,
            SUM(total_price) AS monthly_profit
        FROM view_total_price_table
        GROUP BY EXTRACT(YEAR FROM order_date), EXTRACT(MONTH FROM order_date)
    """)
    conn.execute(query)

def create_view_months(conn):
    query = sql.SQL("""
        CREATE VIEW view_months AS
        SELECT 1 AS m UNION SELECT 2 UNION SELECT 3 UNION SELECT 4
        UNION SELECT 5 UNION SELECT 6 UNION SELECT 7 UNION SELECT 8
        UNION SELECT 9 UNION SELECT 10 UNION SELECT 11 UNION SELECT 12
    """)
    conn.execute(query)