#pragma once


namespace mh {

	//所有需要逆天变量传参的节点都应该继承ClosureNode

	//生成一个静态方法 + 基于ClosureNode的构造方法
#define REGISTER_FROM_CLOSUER(name) REGISTER_FROM(name,	Action*); name(Action* param, uint32_t index, NodePtr parent): ClosureNode(param, index, parent) { } ;

	typedef std::shared_ptr<class ClosureNode> ClosureNodePtr;

	class ClosureNode : public ActionNode {
	public:
		REGISTER_FROM_ACTION(ClosureNode)
	
		//逆天参数区索引
		virtual int getCrossDomainIndex() { return 0; }

		//是否自动传递逆天局部变量  类似闭包里跨域引用
		virtual bool isVariableCrossDomain() {  return true; }

		//是否自动传递 获取触发单位 获取触发玩家 这些函数值
		virtual bool isFunctionCrossDomain() { return true; }

		virtual TYPE getType() override {  return TYPE::CLOSURE; }

		virtual std::string toString(TriggerFunction* func) override { 
			return std::string();
		}
	
		int getCurrentGroupId() { return m_current_group_id; }

		virtual Function* getBlock(TriggerFunction* func, const std::string& closure_name, std::string& upvalues) {
			std::vector<NodePtr> upvalues_scope; //所有参数区的节点
			std::vector<NodePtr> closure_scope;  //所有动作区的节点

			std::map<std::string, std::string> search_upvalue_map;

			m_current_group_id = -1;

			//分类
			for (auto& node : getChildList()) {
				Action* action = (Action*)node->getData();
				if (action->group_id <= getCrossDomainIndex()) {
					upvalues_scope.push_back(node);
				} else {
					closure_scope.push_back(node);
				}
			}

			//生成事件以及参数区的代码 并搜索主动申请的逆天局部变量表
			for (auto& node : upvalues_scope) {
				Action* action = (Action*)node->getData();
				if (action->group_id == getCrossDomainIndex()) { //如果是参数区
					uint32_t id = node->getNameId();
					if (id == "YDWESetAnyTypeLocalArray"s_hash or id == "YDWESetAnyTypeLocalVariable"s_hash) {
						std::string upvalue_name = action->parameters[1]->value;
						std::string upvalue_type = action->parameters[0]->value + 11;
						search_upvalue_map.emplace(upvalue_name, upvalue_type);
					}
				}

				m_current_group_id = action->group_id;
				upvalues += node->toString(func);
			}

			std::map<std::string, Upvalue>* prev_upvalue_map_ptr = nullptr;

			//找到上一层闭包 的逆天局部变量表
			getValue([&](const NodePtr ptr) {
				if (ptr.get() != this) {
					if (ptr->getType() == TYPE::CLOSURE) {
						auto node = std::dynamic_pointer_cast<ClosureNode>(ptr);
						prev_upvalue_map_ptr = &node->upvalue_map;
						return true;
					} 
				}
				return false;
			});
			

			Function* closure = new Function(closure_name, "nothing");
			func->push(closure);
			//生成闭包代码
			for (auto& node : closure_scope) {
				m_current_group_id = ((Action*)node->getData())->group_id;
				*closure << node->toString(func);
			}
			func->pop();

			
			//主动申明在参数区的逆天局部变量 不用上一层处理
			for (auto& [n, t] : search_upvalue_map) {
				upvalue_map.erase(n);
				if (prev_upvalue_map_ptr) {
					prev_upvalue_map_ptr->erase(n);
				}
			}
			
			for (auto&& [n, v] : upvalue_map) {
				//当前这层允许跨域 才生成逆天的自动传参代码。
				if ((!v.is_func && isVariableCrossDomain()) || (v.is_func && isFunctionCrossDomain())) {
					//生成保存状态代码
					Upvalue upvalue = { };
					auto set = Upvalue::TYPE::SET_LOCAL;
					auto get = Upvalue::TYPE::GET_LOCAL;
					//if (v.uptype == Upvalue::TYPE::GET_ARRAY || v.uptype == Upvalue::TYPE::SET_ARRAY) {
					//	set = Upvalue::TYPE::SET_ARRAY;
					//	get = Upvalue::TYPE::GET_ARRAY;
					//}
					upvalue.uptype = set;
					upvalue.name = v.name;
					upvalue.type = v.type;
					upvalue.is_func = v.is_func;
					upvalue.index = v.index;

					m_current_group_id = getCrossDomainIndex();
					upvalue.value = getUpvalue({ get, v.name, v.type, "", v.index, v.is_func });
					upvalues += func->getSpaces() + "call " + getUpvalue(upvalue) + "\n";
					m_current_group_id = -1;

					if (prev_upvalue_map_ptr && prev_upvalue_map_ptr != &upvalue_map) {
						//将通知上一层闭包 让他们保存状态
						prev_upvalue_map_ptr->emplace(n, upvalue);
					}
				}
			}

			upvalue_map.clear();

			m_current_group_id = -1;

			return closure;
		}

		virtual std::string getUpvalue(const Upvalue& info) override  {
			return std::string();
		}


	public:
		//逆天局部变量表
		std::map<std::string, Upvalue> upvalue_map;

		int m_current_group_id = -1;

		bool params_finish;
	
	};
}