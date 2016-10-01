#ifndef COMMON_FACTORY_H
#define COMMON_FACTORY_H
namespace Utility
{
	namespace Factory
	{
		template <class T, class BaseClass, class indexType>
		class FactoryProduct
		{
		public:
			FactoryProduct(indexType index)
			{
				BaseClass::s_factory.template RegisterProduct<T>(index);
			}
			virtual ~FactoryProduct(){};
			static BaseClass *NewProduct()
			{
				return new T;
			}
		};

		template <class BaseClass>
		class FactoryWithIndex
		{
		public:
			FactoryWithIndex(){};
			virtual ~FactoryWithIndex(){
				products.clear();
			}
			typedef BaseClass * (*CreateNewProductDelegate)();

			typedef std::map<unsigned int, CreateNewProductDelegate> ProductsMap;
			ProductsMap products;

			template<class Product>void RegisterProduct(unsigned int index)
			{
				products.insert(make_pair(index, FactoryProduct<Product, BaseClass, unsigned int >::NewProduct));
				//		products[index] = FactoryProduct<Product,BaseClass,unsigned int >::NewProduct;
			}

			void Size()
			{
				return products.size();
			}

			void Clear()
			{
				products.clear();
			}

			BaseClass *CreateNewProduct(unsigned int index)
			{

				typename ProductsMap::iterator findIt = products.find(index);

				if (products.find(index) != products.end())
				{
					return findIt->second();
				}
				else
				{
					return NULL;
				}
			}
		};
#define DeclareIndexToFactory(Product,BaseClass) \
		static FactoryProduct<Product,BaseClass,unsigned int> *factoryProduct;
#define RegisterIndexToFactory(Product,BaseClass,index) \
		FactoryProduct<Product,BaseClass,unsigned int> *Product::factoryProduct = new FactoryProduct<Product,BaseClass,unsigned int>(index);
	}
}
#endif // COMMON_FACTORY_H