DROP TABLE IF EXISTS customer CASCADE;

CREATE TABLE customer
(
    cust_id integer NOT NULL,
    full_name text NOT NULL,
    age integer NOT NULL,
    phone text NOT NULL,
    CONSTRAINT Customer_pkey PRIMARY KEY (cust_id),
    CONSTRAINT CUST_ID_IS_POSITIVE CHECK (cust_id > 0),
    CONSTRAINT LEGAL_AGE CHECK (age >= 18 AND age <= 120),
    CONSTRAINT LEGAL_PHONE CHECK (length(phone) = 10)
);

DROP TABLE IF EXISTS "order" CASCADE;

CREATE TABLE "order"
(
    order_id integer NOT NULL,
    order_date timestamp(0) without time zone NOT NULL,
    delivery_fee numeric NOT NULL,
    delivery_address text NOT NULL,
    CONSTRAINT order_pkey PRIMARY KEY (order_id),
    CONSTRAINT ORDER_ID_IS_POSITIVE CHECK (order_id > 0),
    CONSTRAINT DELIVERY_FEE_NON_NEGATIVE CHECK (delivery_fee >= 0),
    CONSTRAINT LEGAL_DELIVERY_ADDRESS CHECK (length(delivery_address) >= 5)
);

DROP TABLE IF EXISTS dish CASCADE;

CREATE TABLE dish
(
    dish_id integer NOT NULL,
    name text NOT NULL,
    price numeric NOT NULL,
    is_active boolean NOT NULL,
    CONSTRAINT dish_pkey PRIMARY KEY (dish_id),
    CONSTRAINT DISH_ID_IS_POSITIVE CHECK (dish_id > 0),
    CONSTRAINT PRICE_IS_POSITIVE CHECK (price > 0),
    CONSTRAINT LEGAL_NAME CHECK (length(name) >= 4)
);