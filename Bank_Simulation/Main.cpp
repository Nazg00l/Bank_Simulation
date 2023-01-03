#include <iostream>
#include <queue>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <thread>
#include <deque>
#include <fstream>
#include <string>


using std::string;

const short WORKING_HOURS = 7;

const short SERVICE_A_TIME = 3;
const short SERVICE_B_TIME = 10;
const short SERVICE_C_TIME = 15;

const double PROBABILITY_A = 1.0;
const double PROBABILITY_B = 0.4;
const double PROBABILITY_C = 0.6;



enum class EmployeeStatus {
	idle,
	busy
};

std::ostream& operator<<(std::ostream& out, const EmployeeStatus value) {
	if (value == EmployeeStatus::idle)
		return out << "Idle";
	else
		return out << "busy";
}

enum class ServiceType {
	A,
	B,
	C
};


enum class ServiceStatus {
	undone,
	done
};

std::ostream& operator<<(std::ostream& out, const ServiceStatus value) {
	if (value == ServiceStatus::undone)
		return out << "Undone";
	else
		return out << "done";
}

class Service {

public:
	Service(ServiceType type, short serviceTime)
		: type(type), serviceTime(serviceTime), serviceStartTime(-1),
		serviceEndTime(-1), status(ServiceStatus::undone) {}
	Service(ServiceType type, short serviceTime, int serviceStartTime, int serviceEndTime)
		: type(type), serviceTime(serviceTime), serviceStartTime(serviceStartTime),
		serviceEndTime(serviceEndTime), status(ServiceStatus::undone) {}
	void setServiceStartTime(int time) { this->serviceStartTime = time; }
	void setServiceEndTime(int time) { this->serviceEndTime = time; }
	void setStatus(ServiceStatus status) { this->status = status; }
	ServiceType getServiceName() const { return type; }
	int getServiceTime() const { return serviceTime; }
	int getServiceStartTime() const { return serviceStartTime; }
	int getServiceEndTime() const { return serviceEndTime; }
	ServiceStatus getStatus() const { return status; }
	//double getServiceProbability() const { return serviceProbability; }

public:
	bool operator == (const Service& s) const {
		return
			this->type == s.type
			&& this->serviceTime == s.serviceTime;
	}

private:
	ServiceType type;
	short serviceTime;
	int serviceStartTime;
	int serviceEndTime;
	ServiceStatus status;

};

class Employee {
public:
	/*Employee(EmployeeStatus status, Service service)
		: status(status), service(service) {}*/
	Employee(EmployeeStatus status, ServiceType service)
		: status(status), service(service), workCount(0), Id(-1) {}
	Employee(EmployeeStatus status, ServiceType service, short Id)
		: status(status), service(service), workCount(0), Id(Id) {}
	void setStatus(EmployeeStatus status) {
		this->status = status;
		if (this->status == EmployeeStatus::busy) {
			workCount++;
		}
	}
	EmployeeStatus getStatus() { return status; }
	ServiceType getService() const { return service; }
	short getWorkCount() const { return workCount; }
	short getEmployeeId() const { return Id; }

public:
	/*bool operator == (const Employee& e) const {
		return this->status == e.status && this->service == e.service;
	}*/
	bool operator == (const Employee& e) const {
		return this->status == e.status
			&& this->service == e.service
			/*&& this->Id == e.Id*/;
	}


	void operator = (const Employee& e) {
		this->status = e.status;
		this->service = e.service;
		this->Id = e.Id;
	}

private:
	EmployeeStatus status;
	ServiceType service;
	short workCount; // How many customers served by this employee
	short Id;
};


class Customer {

public:
	Customer(int arrivalTime)
		: arrivalTime(arrivalTime), services(new std::vector<Service>) {}
	void setArrivalTime(int time) { this->arrivalTime = time; }
	void addServices(Service service) { services->push_back(service); }
	void setResponsibleEmployeeId(short id) { responsibleEmployeeId = id; }
	int getArrivalTime() const { return arrivalTime; }
	std::vector<Service>& getServices() const { return *services; }
	short getResponsibleEmployeeId() const { return responsibleEmployeeId; }

private:
	int arrivalTime;
	std::vector<Service>* services;
	short responsibleEmployeeId;

};

std::vector<Employee> servicesEmployees;

std::queue<Customer> customerQueue;


void writeFileHeader(std::ofstream& aFile);
void writeToFile(std::ofstream& aFile, Customer& c, ServiceType service);

int main(int argc, char* argv[])
{
	// uniformly-distributed integer random number generator that produces non-deterministic random numbers.  
	std::random_device rd;
	// A Mersenne Twister pseudo-random generator of 32-bit numbers
	std::mt19937 rng(rd());
	// Random number distribution that produces integer values according to a uniform discrete distribution with specified range. 
	//std::uniform_int_distribution<int> arrival_time_distribution(0, WORKING_HOURS * 60);
	std::uniform_int_distribution<int> arrival_time_distribution(0, 60);
	//std::uniform_int_distribution<int> probability_distribution(0, 99);
	std::uniform_real_distribution<double> probability_distribution(0, 0.99);

	short id = 1;
	for (int i = 0; i < 3; ++i) {
		// Adding the two emplyees for A and B services
		if (i <= 1) {
			servicesEmployees.push_back(Employee(EmployeeStatus::idle, ServiceType::A, id));
			++id;
			servicesEmployees.push_back(Employee(EmployeeStatus::idle, ServiceType::B, id));
			++id;
		}
		// Adding the 3 employess for C service
		servicesEmployees.push_back(Employee(EmployeeStatus::idle, ServiceType::C, id));
		++id;
	}

	for (auto e : servicesEmployees) {
		std::cout << (e.getService() == ServiceType::A
			? "A"
			: (e.getService() == ServiceType::B ? "B" : "C")) << "  " << e.getEmployeeId() << std::endl;
	}

	std::vector<int> customers_random_arrival_time;


	int hour_multilpier = 0;
	while (hour_multilpier < WORKING_HOURS) {
		/*switch (minutes){
		case 60: case 120: case 180: case 240: case 300: case 360: case 420:
			++hour_multilpier;
			break;
		default:
			break;
		}
		++minutes;*/
		std::vector<int> temp_arrival_time;
		// Fill "temp_arrival_time" with 25 customer random arival_time with the specified hour by hour_multiplier
		while (temp_arrival_time.size() < 25) {
			auto pushed_value = arrival_time_distribution(rng) + (hour_multilpier * 60);
			// check that there is no duplicate in arrival time values
			if (std::find(temp_arrival_time.begin(), temp_arrival_time.end(), pushed_value)
				== temp_arrival_time.end()) {
				temp_arrival_time.push_back(pushed_value);
			}
		}
		// Append "temp_arrival_time" to customers_random_arrival_time 
		customers_random_arrival_time.reserve(customers_random_arrival_time.size() + temp_arrival_time.size());
		customers_random_arrival_time.insert(customers_random_arrival_time.end(), temp_arrival_time.begin(), temp_arrival_time.end());

		std::cout << "Vector size: " << customers_random_arrival_time.size() << std::endl;
		temp_arrival_time.clear();
		++hour_multilpier;
	}

	// sort the vector in ascending order 
	std::sort(customers_random_arrival_time.begin(), customers_random_arrival_time.end());

	int hour_counter = 0;
	for (auto value : customers_random_arrival_time) {
		std::cout << value << " ";
		++hour_counter;
		switch (hour_counter)
		{
		case 25:
		case 50:
		case 75:
		case 100:
		case 125:
		case 150:
		case 175:
			std::cout << std::endl;
			break;
		default:
			break;
		}
	}

	// TODO: I didn't decided yet how to start but for now I am just filling the customers queue
	for (auto t : customers_random_arrival_time) {

		Customer c(t);
		customerQueue.push(c);
	}

	int minutes = 0;
	bool isJobDone = false;
	//std::vector<Customer> service_A_ServedCustomers;
	std::deque<Customer> service_A_ServedCustomers;
	std::vector<Customer> service_B_C_OngoingCustomers;
	std::vector<Customer> service_B_C_ServedCustomers;
	std::ofstream aFile("A_Customers.txt"), bFile("B_Customers.txt"),
		cFile("C_Customers.txt"), allFile("All_Customers.txt");

	writeFileHeader(aFile);
	writeFileHeader(bFile);
	writeFileHeader(cFile);
	writeFileHeader(allFile);

	while (!isJobDone) {

		if (!service_A_ServedCustomers.empty()) {
			for (Customer& c : service_A_ServedCustomers) {
				for (Service& s : c.getServices()) {
					if (s.getServiceName() == ServiceType::A && s.getServiceEndTime() == minutes) {
						// TODO: the service is done now 
						s.setStatus(ServiceStatus::done);
						/*auto busyEmployee =
							std::find(servicesEmployees.begin(), servicesEmployees.end(), Employee(EmployeeStatus::busy, ServiceType::A, c.employeeId));*/
						for (auto& e : servicesEmployees) {
							if (c.getResponsibleEmployeeId() == e.getEmployeeId()) {
								e.setStatus(EmployeeStatus::idle);
								break;
							}
						}
						//busyEmployee->setStatus(EmployeeStatus::idle);
					}
				}
			}
		}

		if (!customerQueue.empty()) {

			auto idleEmployee = servicesEmployees.begin();
			// Loop to find all the idle employees
			while (idleEmployee != servicesEmployees.end()) {
				idleEmployee =
					std::find(servicesEmployees.begin(), servicesEmployees.end(), Employee(EmployeeStatus::idle, ServiceType::A));
				if (idleEmployee != servicesEmployees.end()) {
					// There is an idle employee
					idleEmployee->setStatus(EmployeeStatus::busy);
					Customer c = customerQueue.front();
					Service s = Service(ServiceType::A, SERVICE_A_TIME);
					///
					/// TODO: the customer shouldn't be added to the queue also until its arrivalTime == minutes(current time)
					/// 
					int startTime = c.getArrivalTime();
					int endTime = startTime + s.getServiceTime();
					s.setServiceStartTime(startTime);
					s.setServiceEndTime(endTime);
					c.setResponsibleEmployeeId(idleEmployee->getEmployeeId());
					c.addServices(s);
					// -------------------

					////////////////////////////////////////////////////
					// Writing to File
					writeToFile(aFile, c, ServiceType::A);
					writeToFile(allFile, c, ServiceType::A);
					// ------------------------
					service_A_ServedCustomers.push_back(c);
					customerQueue.pop();
				}
			}

			//for (auto it = servicesEmployees.begin(); it != servicesEmployees.end(); ++it) {
			//	if (it->getService() == ServiceType::A && it->getStatus() == EmployeeStatus::idle) {
			//		// There is an idle employee
			//		it->setStatus(EmployeeStatus::busy);
			//		std::cout << "Emp name: " << it->employeeName << std::endl;
			//		Customer c = customerQueue.front();
			//		Service s = Service(ServiceType::A, SERVICE_A_TIME);
			//		///
			//		/// TODO: the customer shouldn't be added to the queue also until its arrivalTime == minutes(current time)
			//		/// 
			//		int startTime = c.getArrivalTime();
			//		int endTime = startTime + s.getServiceTime();
			//		s.setServiceStartTime(startTime);
			//		s.setServiceEndTime(endTime);
			//		c.employeeId = it->Id;
			//		//c.setResponsibleEmployee(&*it);
			//		c.addServices(s);
			//		// -------------------

			//		////////////////////////////////////////////////////
			//		// Writing to File
			//		writeToFile(aFile, c, ServiceType::A);
			//		writeToFile(allFile, c, ServiceType::A);
			//		// ------------------------
			//		service_A_ServedCustomers.push_back(c);
			//		customerQueue.pop();
			//	}
			//}
		}



		///
		/// Now serving B and C Services
		/// 


		if (!service_B_C_OngoingCustomers.empty()) {
			std::vector<Customer>::iterator customerIterator = service_B_C_OngoingCustomers.begin();
			while (customerIterator != service_B_C_OngoingCustomers.end()) {
				for (Service& s : customerIterator->getServices()) {
					if ((s.getServiceName() == ServiceType::B || s.getServiceName() == ServiceType::C)
						&& s.getServiceEndTime() == minutes) {
						// TODO: the service is done now 
						s.setStatus(ServiceStatus::done);
						// Get the customers chosen Service B or C
						auto serviceName = s.getServiceName();
						/*auto busyEmployee =
							std::find(servicesEmployees.begin(), servicesEmployees.end(), Employee(EmployeeStatus::busy, serviceName));*/
						for (auto& e : servicesEmployees) {
							if (customerIterator->getResponsibleEmployeeId() == e.getEmployeeId()) {
								e.setStatus(EmployeeStatus::idle);
								break;
							}
						}
						//busyEmployee->setStatus(EmployeeStatus::idle);
						service_B_C_ServedCustomers.push_back(*customerIterator);
						//customerIterator = service_B_C_OngoingCustomers.erase(customerIterator);
					}
				}
				++customerIterator;
			}
			//for (Customer& c : service_B_C_OngoingCustomers) {
			//	for (Service& s : c.getServices()) {
			//		if ((s.getServiceName() == ServiceType::B || s.getServiceName() == ServiceType::C)
			//			&& s.getServiceEndTime() == minutes) {
			//			// TODO: the service is done now 
			//			s.setStatus(ServiceStatus::done);
			//			// Get the customers chosen Service B or C
			//			auto serviceName = s.getServiceName();
			//			auto busyEmployee =
			//				std::find(servicesEmployees.begin(), servicesEmployees.end(), Employee(EmployeeStatus::busy, serviceName));
			//			busyEmployee->setStatus(EmployeeStatus::idle);
			//			service_B_C_ServedCustomers.push_back(c);
			//			
			//		}
			//	}
			//}
		}


		if (!service_A_ServedCustomers.empty() && service_A_ServedCustomers.front().getServices()[0].getStatus() == ServiceStatus::done) {
			auto idleEmployee = servicesEmployees.begin();
			// Loop to find all the idle employees
			while (idleEmployee != servicesEmployees.end() && !service_A_ServedCustomers.empty() && service_A_ServedCustomers.front().getServices()[0].getStatus() == ServiceStatus::done) {
				Customer c = service_A_ServedCustomers.front();
				double probability = probability_distribution(rng);
				/** The Customer choosed B Service*/
				if (probability <= PROBABILITY_B) {
					idleEmployee =
						std::find(servicesEmployees.begin(), servicesEmployees.end(), Employee(EmployeeStatus::idle, ServiceType::B));
					if (idleEmployee != servicesEmployees.end()) {
						// There is an idle employee
						//idleEmployee->setStatus(EmployeeStatus::busy);
						//Customer c = serviceAServedCustomers.front();
						if (c.getServices()[0].getStatus() == ServiceStatus::done) {
							idleEmployee->setStatus(EmployeeStatus::busy);
							Service s = Service(ServiceType::B, SERVICE_B_TIME);
							///
							/// TODO: the customer shouldn't be added to the queue also until its arrivalTime == minutes(current time)
							/// 
							int startTime = minutes;
							int endTime = startTime + s.getServiceTime();
							s.setServiceStartTime(startTime);
							s.setServiceEndTime(endTime);
							c.setResponsibleEmployeeId(idleEmployee->getEmployeeId());
							c.addServices(s);
							////////////////////////////////////////////////////
							// Writing to File
							writeToFile(bFile, c, ServiceType::B);
							writeToFile(allFile, c, ServiceType::B);
							//--------------------------------------------------
							service_B_C_OngoingCustomers.push_back(c);
							service_A_ServedCustomers.pop_front();
						}
					}
				}
				/** The Customer choosed C Service*/
				else {
					idleEmployee =
						std::find(servicesEmployees.begin(), servicesEmployees.end(), Employee(EmployeeStatus::idle, ServiceType::C));
					if (idleEmployee != servicesEmployees.end()) {
						// There is an idle employee
						//idleEmployee->setStatus(EmployeeStatus::busy);
						//Customer c = serviceAServedCustomers.front();
						if (c.getServices()[0].getStatus() == ServiceStatus::done) {
							idleEmployee->setStatus(EmployeeStatus::busy);
							Service s = Service(ServiceType::C, SERVICE_C_TIME);
							///
							/// TODO: the customer shouldn't be added to the queue also until its arrivalTime == minutes(current time)
							/// 
							int startTime = minutes;
							int endTime = startTime + s.getServiceTime();
							s.setServiceStartTime(startTime);
							s.setServiceEndTime(endTime);
							c.setResponsibleEmployeeId(idleEmployee->getEmployeeId());
							c.addServices(s);
							////////////////////////////////////////////////////
							// Writing to File
							writeToFile(cFile, c, ServiceType::C);
							writeToFile(allFile, c, ServiceType::C);
							//--------------------------------------------------
							service_B_C_OngoingCustomers.push_back(c);
							service_A_ServedCustomers.pop_front();
						}
					}
				}
			}
		}

		/*if (!customerQueue.empty()) {
			int arrivalTime = customerQueue.front().getArrivalTime();
			if (arrivalTime == minutes) {
				std::cout << "Mezzo is trying out: " << arrivalTime << std::endl;
			}
			int arrivalTime2 = service_A_ServedCustomers.front().getArrivalTime();
			if (arrivalTime2 == minutes) {
				std::cout << "Mezzo is trying out: " << arrivalTime << std::endl;
			}
		}*/


		if (service_A_ServedCustomers.empty() && service_B_C_ServedCustomers.size() == customers_random_arrival_time.size()) {
			isJobDone = true;
			aFile.close();
			bFile.close();
			cFile.close();
			allFile.close();
		}
		++minutes;
	}

	std::cout << "SERVICE A: SERVED CUSTOMERS: " << std::endl;
	for (auto c : service_A_ServedCustomers) {
		std::cout << "Customer AT: " << c.getArrivalTime()
			<< " , End time: " << c.getServices().front().getServiceEndTime()
			<< " status: " << c.getServices().front().getStatus() << std::endl;
	}
	std::cout << "====================================" << std::endl;
	std::cout << "====================================" << std::endl;
	std::cout << "SERVICE B and C: SERVED CUSTOMERS: " << std::endl;
	for (auto c : service_B_C_OngoingCustomers) {
		std::cout << "Customer AT: " << c.getServices().front().getServiceStartTime()
			<< " , Start Time: " << c.getServices()[1].getServiceStartTime()
			<< " , End time: " << c.getServices()[1].getServiceEndTime()
			<< " status: " << c.getServices()[1].getStatus() << std::endl;
	}

	std::cout << "B,C Vector size: " << service_B_C_OngoingCustomers.size() << std::endl;


	std::cout << "4 Files created with: " << std::endl
		<< "---------------------" << std::endl
		<< "A_Customers.txt: contains all the customers with \"A\" service served." << std::endl
		<< "B_Customers.txt: contains all the customers with \"B\" service served." << std::endl
		<< "C_Customers.txt: contains all the customers with \"C\" service served." << std::endl
		<< "All_Customers.txt: contains all the customers with all the  services served." << std::endl
		<< "<<<<<<<==============**********************************==============>>>>>>>\n" << std::endl;


	/**
	* Printing statistics
	*/

	std::cout << "Services employees average working hours: " << std::endl;
	std::cout << "==========================================" << std::endl;
	std::cout << "==========================================" << std::endl;

	double employeeA_avg, employeeB_avg, employeeC_avg;
	short empNum = 1;
	for (const auto& e : servicesEmployees) {
		if (e.getService() == ServiceType::A) {
			double hours = ((double)e.getWorkCount() * SERVICE_A_TIME) / 60;
			employeeA_avg = double(hours) / (double)WORKING_HOURS;
			std::cout << "Service A Employee #" << empNum << ": " << std::endl;
			std::cout << "------------------------" << std::endl;
			std::cout << "Served " << e.getWorkCount() << "  Customers" << std::endl;
			std::cout << "Worked for: " << hours << " hours" << std::endl;
			std::cout << "Average out of " << WORKING_HOURS << " hours of working: " << employeeA_avg << std::endl << std::endl;
			++empNum;
		}
	}
	empNum = 1;
	for (const auto& e : servicesEmployees) {
		if (e.getService() == ServiceType::A) {
			double hours = ((double)e.getWorkCount() * SERVICE_A_TIME) / 60;
			employeeB_avg = double(hours) / (double)WORKING_HOURS;
			std::cout << "Service B Employee #" << empNum << ": " << std::endl;
			std::cout << "------------------------" << std::endl;
			std::cout << "Served " << e.getWorkCount() << "  Customers" << std::endl;
			std::cout << "Worked for: " << hours << " hours" << std::endl;
			std::cout << "Average out of " << WORKING_HOURS << " hours of working: " << employeeB_avg << std::endl << std::endl;
			++empNum;
		}
	}
	empNum = 1;
	for (const auto& e : servicesEmployees) {
		if (e.getService() == ServiceType::A) {
			double hours = ((double)e.getWorkCount() * SERVICE_A_TIME) / 60;
			employeeC_avg = double(hours) / (double)WORKING_HOURS;
			std::cout << "Service C Employee #" << empNum << ": " << std::endl;
			std::cout << "------------------------" << std::endl;
			std::cout << "Served " << e.getWorkCount() << "  Customers" << std::endl;
			std::cout << "Worked for: " << hours << " hours" << std::endl;
			std::cout << "Average out of " << WORKING_HOURS << " hours of working: " << employeeC_avg << std::endl << std::endl;
			++empNum;
		}
	}

}


void writeFileHeader(std::ofstream& aFile) {
	if (aFile.is_open()) {
		aFile << "Arrival Time\tStart Time\tEnd Time\tService\tA1\tA2\tB1\tB2\tC1\tC2\tC3" << std::endl;
		aFile << "------------\t----------\t--------\t-------\t--\t--\t--\t--\t--\t--\t--" << std::endl;
	}
	else {
		std::cout << "Unable to open the file." << std::endl;
	}
}

void writeToFile(std::ofstream& aFile, Customer& c, ServiceType service) {
	if (aFile.is_open()) {

		if (service == ServiceType::A) {
			aFile << std::to_string(c.getArrivalTime()) << "\t\t"
				<< std::to_string(c.getServices()[0].getServiceStartTime()) << "\t\t"
				<< std::to_string(c.getServices()[0].getServiceEndTime()) << "\t\t"
				<< "A" << "\t";
		}
		else {
			aFile << std::to_string(c.getArrivalTime()) << "\t\t"
				<< std::to_string(c.getServices()[1].getServiceStartTime()) << "\t\t"
				<< std::to_string(c.getServices()[1].getServiceEndTime()) << "\t\t"
				<< (service == ServiceType::B ? "B" : "C") << "\t";
		}

		// A Service employees
		for (auto e : servicesEmployees) {
			if (e.getService() == ServiceType::A) {
				aFile << (e.getStatus() == EmployeeStatus::idle ? "Idle" : "Busy") << "\t";
			}
		}
		// B Service employees
		for (auto e : servicesEmployees) {
			if (e.getService() == ServiceType::B) {
				aFile << (e.getStatus() == EmployeeStatus::idle ? "Idle" : "Busy") << "\t";
			}
		}
		// C Service employees
		for (auto e : servicesEmployees) {
			if (e.getService() == ServiceType::C) {
				aFile << (e.getStatus() == EmployeeStatus::idle ? "Idle" : "Busy") << "\t";
			}
		}

		aFile << std::endl;
	}
	else {
		std::cout << "Unable to open the file." << std::endl;
	}
}

//void writeToAFile(std::ofstream& aFile, Customer& c) {
//	if (aFile.is_open()) {
//		aFile << std::to_string(c.getArrivalTime()) << "\t\t"
//			<< std::to_string(c.getServices()[0].getServiceStartTime()) << "\t\t"
//			<< std::to_string(c.getServices()[0].getServiceEndTime()) << "\t\t"
//			<< "A" << "\t"; /*servicesEmployees*/
//			//<< std::endl;
//
//		for (auto e : servicesEmployees) {
//			aFile << (e.getStatus() == EmployeeStatus::idle ? "Idle" : "Busy") << "\t";
//		}
//		//std::find_if(servicesEmployees.begin(), servicesEmployees.end(), 
//		//	[&](Employee e) {
//		//	//Employee(EmployeeStatus::idle, ServiceType::A);
//		//	if (e.getService() == ServiceType::A) {
//		//		aFile << (e.getStatus() == EmployeeStatus::idle ? "Idle" : "Busy");
//		//	}
//		//	return e.getService() == ServiceType::A;
//		//	});
//
//		aFile << std::endl;
//	}
//	else {
//		std::cout << "Unable to open the file." << std::endl;
//	}
//}
