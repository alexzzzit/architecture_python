// data.js — Скрипт загрузки тестовых данных для MongoDB
// Запуск: mongosh "mongodb://localhost:27017/medical_db" < data.js
// Или внутри контейнера: docker compose exec mongodb mongosh medical_db < data.js

db = db.getSiblingDB('medical_db');

// Очистка коллекций (для повторного запуска)
db.users.deleteMany({});
db.patients.deleteMany({});
db.medical_records.deleteMany({});
db.counters.deleteMany({});

// Инициализация счётчиков для автоинкремента id
db.counters.insertMany([
  { _id: "users", seq: 0 },
  { _id: "patients", seq: 0 },
  { _id: "medical_records", seq: 0 }
]);

// Вспомогательная функция для получения следующего уникального ID
function getNextId(collectionName) {
  const result = db.counters.findAndModify({
    query: { _id: collectionName },
    update: { $inc: { seq: 1 } },
    new: true
  });
  return result.seq;
}

// =============================================================================
// USERS (10 документов)
// =============================================================================
const users = [
  {
    id: NumberLong(getNextId("users")),
    login: "doctor_smirnov",
    email: "smirnov@hospital.ru",
    password_hash: "$2b$10$abc123def456",
    role: "doctor",
    is_active: true,
    created_at: new Date("2024-01-15T08:00:00Z"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "patient_ivanov",
    email: "ivanov@mail.ru",
    password_hash: "$2b$10$ghi789jkl012",
    role: "patient",
    is_active: true,
    created_at: new Date("2024-02-01T10:30:00Z"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "patient_petrova",
    email: "petrova@example.com",
    password_hash: "$2b$10$mno345pqr678",
    role: "patient",
    is_active: true,
    created_at: new Date("2024-02-10T14:20:00Z"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "admin_root",
    email: "admin@hospital.ru",
    password_hash: "$2b$10$stu901vwx234",
    role: "admin",
    is_active: true,
    created_at: new Date("2024-01-01T00:00:00Z"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "lab_kozlov",
    email: "kozlov@lab.ru",
    password_hash: "$2b$10$yza567bcd890",
    role: "lab_technician",
    is_active: true,
    created_at: new Date("2024-01-20T09:15:00Z"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "patient_sidorov",
    email: "sidorov@test.ru",
    password_hash: "$2b$10$efg123hij456",
    role: "patient",
    is_active: true,
    created_at: new Date("2024-03-01T11:00:00Z"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "doctor_volkova",
    email: "volkova@hospital.ru",
    password_hash: "$2b$10$klm789nop012",
    role: "doctor",
    is_active: true,
    created_at: new Date("2024-01-25T13:45:00Z"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "patient_kuznetsova",
    email: "kuznetsova@mail.ru",
    password_hash: "$2b$10$qrs345tuv678",
    role: "patient",
    is_active: false,
    created_at: new Date("2024-02-20T16:30:00Z"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "patient_popov",
    email: "popov@example.com",
    password_hash: "$2b$10$wxy901zab234",
    role: "patient",
    is_active: true,
    created_at: new Date("2024-03-10T08:20:00Z"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "doctor_novikov",
    email: "novikov@hospital.ru",
    password_hash: "$2b$10$cde567fgh890",
    role: "doctor",
    is_active: true,
    created_at: new Date("2024-02-05T10:00:00Z"),
    updated_at: new Date()
  }
];
db.users.insertMany(users);
print("Загружено пользователей: " + db.users.countDocuments({}));

// =============================================================================
// PATIENTS (10 документов)
// =============================================================================
const patients = [
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(2),
    policy_number: "1234567890123456",
    snils: "123-456-789 00",
    first_name: "Иван",
    last_name: "Иванов",
    middle_name: "Иванович",
    birth_date: new Date("1985-06-15"),
    phone: "+79001234567",
    address: {
      city: "Москва",
      street: "Тверская",
      building: "10",
      apartment: "25"
    },
    emergency_contact: {
      name: "Мария Иванова",
      phone: "+79007654321",
      relation: "супруга"
    },
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(3),
    policy_number: "6543210987654321",
    snils: "987-654-321 00",
    first_name: "Анна",
    last_name: "Петрова",
    middle_name: "Сергеевна",
    birth_date: new Date("1992-03-22"),
    phone: "+79009876543",
    address: {
      city: "Санкт-Петербург",
      street: "Невский проспект",
      building: "28",
      apartment: "12"
    },
    emergency_contact: {
      name: "Пётр Петров",
      phone: "+79001112233",
      relation: "супруг"
    },
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(6),
    policy_number: "1111222233334444",
    snils: "111-222-333 44",
    first_name: "Ольга",
    last_name: "Сидорова",
    middle_name: "Петровна",
    birth_date: new Date("1978-11-30"),
    phone: "+79001112233",
    address: {
      city: "Казань",
      street: "Баумана",
      building: "15",
      apartment: "8"
    },
    emergency_contact: {
      name: "Иван Сидоров",
      phone: "+79003334455",
      relation: "брат"
    },
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(8),
    policy_number: "5555666677778888",
    snils: "555-666-777 88",
    first_name: "Елена",
    last_name: "Кузнецова",
    middle_name: "Александровна",
    birth_date: new Date("1990-07-14"),
    phone: "+79005556677",
    address: {
      city: "Новосибирск",
      street: "Ленина",
      building: "42",
      apartment: "101"
    },
    emergency_contact: {
      name: "Дмитрий Кузнецов",
      phone: "+79007778899",
      relation: "супруг"
    },
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(9),
    policy_number: "9999000011112222",
    snils: "999-000-111 22",
    first_name: "Алексей",
    last_name: "Попов",
    middle_name: "Михайлович",
    birth_date: new Date("1988-01-25"),
    phone: "+79009990011",
    address: {
      city: "Екатеринбург",
      street: "Малышева",
      building: "5",
      apartment: "33"
    },
    emergency_contact: {
      name: "Наталья Попова",
      phone: "+79001110022",
      relation: "супруга"
    },
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(10),
    policy_number: "2222333344445555",
    snils: "222-333-444 55",
    first_name: "Дмитрий",
    last_name: "Смирнов",
    middle_name: "Андреевич",
    birth_date: new Date("1995-09-10"),
    phone: "+79002223344",
    address: {
      city: "Нижний Новгород",
      street: "Большая Покровская",
      building: "20",
      apartment: "15"
    },
    emergency_contact: {
      name: "Ольга Смирнова",
      phone: "+79004445566",
      relation: "сестра"
    },
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(11),
    policy_number: "3333444455556666",
    snils: "333-444-555 66",
    first_name: "Мария",
    last_name: "Волкова",
    middle_name: "Игоревна",
    birth_date: new Date("1983-04-18"),
    phone: "+79003334455",
    address: {
      city: "Самара",
      street: "Молодогвардейская",
      building: "100",
      apartment: "50"
    },
    emergency_contact: {
      name: "Игорь Волков",
      phone: "+79005556677",
      relation: "супруг"
    },
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(12),
    policy_number: "4444555566667777",
    snils: "444-555-666 77",
    first_name: "Сергей",
    last_name: "Новиков",
    middle_name: "Владимирович",
    birth_date: new Date("1991-12-05"),
    phone: "+79004445566",
    address: {
      city: "Омск",
      street: "Ленина",
      building: "8",
      apartment: "22"
    },
    emergency_contact: {
      name: "Татьяна Новикова",
      phone: "+79006667788",
      relation: "супруга"
    },
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(13),
    policy_number: "6666777788889999",
    snils: "666-777-888 99",
    first_name: "Наталья",
    last_name: "Морозова",
    middle_name: "Евгеньевна",
    birth_date: new Date("1987-08-20"),
    phone: "+79006667788",
    address: {
      city: "Челябинск",
      street: "Кирова",
      building: "30",
      apartment: "45"
    },
    emergency_contact: {
      name: "Евгений Морозов",
      phone: "+79008889900",
      relation: "супруг"
    },
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(14),
    policy_number: "7777888899990000",
    snils: "777-888-999 00",
    first_name: "Андрей",
    last_name: "Лебедев",
    middle_name: "Сергеевич",
    birth_date: new Date("1994-02-28"),
    phone: "+79007778899",
    address: {
      city: "Уфа",
      street: "Цюрупы",
      building: "12",
      apartment: "7"
    },
    emergency_contact: {
      name: "Светлана Лебедева",
      phone: "+79009990011",
      relation: "сестра"
    },
    created_at: new Date(),
    updated_at: new Date()
  }
];
db.patients.insertMany(patients);
print("Загружено пациентов: " + db.patients.countDocuments({}));

// =============================================================================
// MEDICAL_RECORDS (12 документов)
// =============================================================================
const records = [
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-001",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "Острый бронхит",
    symptoms: ["кашель", "температура 37.8", "слабость"],
    treatment_plan: {
      medications: [
        { name: "Амброксол", dosage: "30мг", frequency: "3 раза в день" },
        { name: "Парацетамол", dosage: "500мг", frequency: "при температуре" }
      ],
      procedures: ["ингаляции", "постельный режим"],
      recommendations: "Обильное тёплое питьё, избегать переохлаждения"
    },
    attachments: [
      {
        filename: "xray_chest_20240301.jpg",
        url: "s3://bucket/records/001/xray.jpg",
        uploaded_at: new Date("2024-03-01T10:30:00Z")
      }
    ],
    status: "confirmed",
    visit_date: new Date("2024-03-01T10:30:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-002",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "Профилактический осмотр",
    symptoms: [],
    treatment_plan: {
      medications: [],
      procedures: ["анализ крови", "ЭКГ"],
      recommendations: "Повторный визит через 6 месяцев"
    },
    attachments: [],
    status: "confirmed",
    visit_date: new Date("2024-06-15T14:00:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-003",
    patient_id: NumberLong(2),
    doctor_id: NumberLong(1),
    diagnosis: "Гипертония 1 степени",
    symptoms: ["повышенное давление", "головная боль"],
    treatment_plan: {
      medications: [
        { name: "Лизиноприл", dosage: "10мг", frequency: "1 раз в день утром" }
      ],
      procedures: ["мониторинг АД"],
      recommendations: "Ограничение соли, контроль веса"
    },
    attachments: [],
    status: "confirmed",
    visit_date: new Date("2024-04-10T09:15:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-004",
    patient_id: NumberLong(2),
    doctor_id: NumberLong(7),
    diagnosis: "ОРВИ",
    symptoms: ["насморк", "боль в горле", "температура 37.2"],
    treatment_plan: {
      medications: [
        { name: "Ибупрофен", dosage: "200мг", frequency: "при боли" },
        { name: "Аквамарис", dosage: "по инструкции", frequency: "3 раза в день" }
      ],
      procedures: ["полоскание горла"],
      recommendations: "Домашний режим 3-5 дней"
    },
    attachments: [],
    status: "confirmed",
    visit_date: new Date("2024-05-20T11:00:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-005",
    patient_id: NumberLong(3),
    doctor_id: NumberLong(1),
    diagnosis: "Остеохондроз поясничного отдела",
    symptoms: ["боль в спине", "ограничение подвижности"],
    treatment_plan: {
      medications: [
        { name: "Диклофенак", dosage: "50мг", frequency: "2 раза в день" }
      ],
      procedures: ["ЛФК", "массаж", "физиотерапия"],
      recommendations: "Избегать подъёма тяжестей, спать на ортопедическом матрасе"
    },
    attachments: [
      {
        filename: "mri_spine_20240415.pdf",
        url: "s3://bucket/records/005/mri.pdf",
        uploaded_at: new Date("2024-04-15T13:20:00Z")
      }
    ],
    status: "confirmed",
    visit_date: new Date("2024-04-15T13:20:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-006",
    patient_id: NumberLong(4),
    doctor_id: NumberLong(7),
    diagnosis: "Гастрит",
    symptoms: ["боль в эпигастрии", "изжога", "тошнота"],
    treatment_plan: {
      medications: [
        { name: "Омепразол", dosage: "20мг", frequency: "1 раз в день утром" },
        { name: "Альмагель", dosage: "по инструкции", frequency: "3 раза в день" }
      ],
      procedures: ["ФГДС"],
      recommendations: "Диета стол №1, исключить острое и жареное"
    },
    attachments: [],
    status: "confirmed",
    visit_date: new Date("2024-03-25T10:00:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-007",
    patient_id: NumberLong(5),
    doctor_id: NumberLong(10),
    diagnosis: "Сахарный диабет 2 типа",
    symptoms: ["жажда", "частое мочеиспускание", "слабость"],
    treatment_plan: {
      medications: [
        { name: "Метформин", dosage: "500мг", frequency: "2 раза в день" }
      ],
      procedures: ["контроль глюкозы", "анализ на гликированный гемоглобин"],
      recommendations: "Диета с ограничением углеводов, физическая активность"
    },
    attachments: [
      {
        filename: "blood_glucose_20240501.csv",
        url: "s3://bucket/records/007/glucose.csv",
        uploaded_at: new Date("2024-05-01T08:00:00Z")
      }
    ],
    status: "confirmed",
    visit_date: new Date("2024-05-01T08:00:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-008",
    patient_id: NumberLong(6),
    doctor_id: NumberLong(1),
    diagnosis: "Аллергический ринит",
    symptoms: ["чихание", "заложенность носа", "слезотечение"],
    treatment_plan: {
      medications: [
        { name: "Цетиризин", dosage: "10мг", frequency: "1 раз в день" },
        { name: "Назонекс", dosage: "по инструкции", frequency: "1 раз в день" }
      ],
      procedures: ["аллергопробы"],
      recommendations: "Избегать контакта с аллергенами, промывание носа"
    },
    attachments: [],
    status: "confirmed",
    visit_date: new Date("2024-04-05T14:30:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-009",
    patient_id: NumberLong(7),
    doctor_id: NumberLong(7),
    diagnosis: "Анемия железодефицитная",
    symptoms: ["слабость", "головокружение", "бледность"],
    treatment_plan: {
      medications: [
        { name: "Сорбифер Дурулес", dosage: "1 таблетка", frequency: "2 раза в день" }
      ],
      procedures: ["общий анализ крови", "анализ на ферритин"],
      recommendations: "Диета с повышенным содержанием железа, контроль анализов через 1 месяц"
    },
    attachments: [
      {
        filename: "cbc_20240320.pdf",
        url: "s3://bucket/records/009/cbc.pdf",
        uploaded_at: new Date("2024-03-20T09:00:00Z")
      }
    ],
    status: "confirmed",
    visit_date: new Date("2024-03-20T09:00:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-010",
    patient_id: NumberLong(8),
    doctor_id: NumberLong(10),
    diagnosis: "Пневмония правосторонняя",
    symptoms: ["кашель с мокротой", "температура 38.5", "одышка"],
    treatment_plan: {
      medications: [
        { name: "Амоксициллин", dosage: "500мг", frequency: "3 раза в день" },
        { name: "Ацетилцистеин", dosage: "200мг", frequency: "3 раза в день" }
      ],
      procedures: ["рентген лёгких", "контрольный анализ крови"],
      recommendations: "Постельный режим, обильное питьё, контроль температуры"
    },
    attachments: [
      {
        filename: "xray_lungs_20240510.jpg",
        url: "s3://bucket/records/010/xray.jpg",
        uploaded_at: new Date("2024-05-10T11:30:00Z")
      }
    ],
    status: "confirmed",
    visit_date: new Date("2024-05-10T11:30:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-011",
    patient_id: NumberLong(9),
    doctor_id: NumberLong(1),
    diagnosis: "Мигрень",
    symptoms: ["головная боль пульсирующего характера", "светобоязнь", "тошнота"],
    treatment_plan: {
      medications: [
        { name: "Суматриптан", dosage: "50мг", frequency: "при приступе" }
      ],
      procedures: ["МРТ головного мозга"],
      recommendations: "Вести дневник головной боли, избегать триггеров"
    },
    attachments: [],
    status: "draft",
    visit_date: new Date("2024-06-01T15:00:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-012",
    patient_id: NumberLong(10),
    doctor_id: NumberLong(7),
    diagnosis: "Дерматит атопический",
    symptoms: ["зуд", "покраснение кожи", "шелушение"],
    treatment_plan: {
      medications: [
        { name: "Эмолиум", dosage: "по инструкции", frequency: "2 раза в день" },
        { name: "Лоратадин", dosage: "10мг", frequency: "1 раз в день" }
      ],
      procedures: ["консультация дерматолога"],
      recommendations: "Использовать гипоаллергенные средства, избегать горячей воды"
    },
    attachments: [],
    status: "confirmed",
    visit_date: new Date("2024-05-25T10:30:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  }
];
db.medical_records.insertMany(records);
print("Загружено медицинских записей: " + db.medical_records.countDocuments({}));

print("\nВсе тестовые данные загружены успешно!");
print("Коллекции:");
print("  - users: " + db.users.countDocuments({}));
print("  - patients: " + db.patients.countDocuments({}));
print("  - medical_records: " + db.medical_records.countDocuments({}));